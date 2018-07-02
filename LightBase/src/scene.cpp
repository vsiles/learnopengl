#include <iostream>
/* #define GLM_FORCE_MESSAGES */
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "cube.hpp"
#include "scene.hpp"

using namespace std;
using namespace vinz;

Scene::Scene(string name, int width, int height) : camera()
{
    int ret = SDL_Init(SDL_INIT_VIDEO);

    if (ret != 0) {
        throw SceneFailure(SceneFailure::Cause::Init, SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    this->width = width;
    this->height = height;

    window = SDL_CreateWindow(name.c_str(),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |
                              SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        const char *err = SDL_GetError();
        SDL_Quit();
        throw SceneFailure(SceneFailure::Cause::Window, err);
    }

    glcontext = SDL_GL_CreateContext(window);
    if (glcontext == nullptr) {
        const char *err = SDL_GetError();
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw SceneFailure(SceneFailure::Cause::Context, err);
    }

    if (gl3wInit()) {
        throw SceneFailure(SceneFailure::Cause::Extension, "gl3w");
    }

    if (!gl3wIsSupported(4, 2)) {
        throw SceneFailure(SceneFailure::Cause::Extension, "4.2");
    }
}

Scene::~Scene()
{
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Scene::updateProjView()
{
    GLfloat w = (GLfloat)width;
    GLfloat h = (GLfloat)height;
    GLfloat aspect = w / h;
    /* TODO: fov, far, near */
    projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

void Scene::run()
{
    updateProjView();
    Cube cube;
    cube.init();

    /* uncomment to draw in wireframe mode */
    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_CaptureMouse(SDL_TRUE);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (true) {
        bool done = processEvent();
        if (done)
            break;

        float currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        const glm::vec3 &pos = camera.getPosition();
        shader.setVec3("viewPos", pos.x, pos.y, pos.z);

        /* Projection matrix is updated when needed */
        shader.setMat4("projection", glm::value_ptr(projection));
        glm::mat4 view = camera.view();
        shader.setMat4("view", glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", glm::value_ptr(model));

        /* Draw cube */
        cube.render();

        lampShader.activate();
        lampShader.setMat4("projection", glm::value_ptr(projection));
        lampShader.setMat4("view", glm::value_ptr(view));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", glm::value_ptr(model));
        cube.render();

        SDL_GL_SwapWindow(window);
    }

}

bool Scene::init(string &res_path)
{
    string vertex_shader = res_path + "shaders/lighttest-2.vert";
    string fragment_shader = res_path + "shaders/lighttest-2.frag";
    string lamp_fragment_shader = res_path + "shaders/lamp.frag";

    try {
        cerr << "Vertex shader compiling..." << endl;
        Shader vert(vertex_shader, Shader::Type::Vertex);
        cerr << "Fragment shader compiling..." << endl;
        Shader frag(fragment_shader, Shader::Type::Fragment);
        cerr << "Fragment shader (lamp) compiling..." << endl;
        Shader lfrag(lamp_fragment_shader, Shader::Type::Fragment);
        ShaderFailure cause;
        if (!shader.init(vert, frag, cause))
            throw cause;
        if (!lampShader.init(vert, lfrag, cause))
            throw cause;
    } catch (ShaderFailure &excp) {
        switch (excp.cause) {
            case ShaderFailure::Cause::Create:
                cerr << "Shader creation failed" << endl;
                break;
            case ShaderFailure::Cause::Compile:
                cerr << "Shader compilation failed:" << endl;
                cerr << excp.log << endl;
                break;
            case ShaderFailure::Cause::IO:
                cerr << "Can't access shader file" << endl;
                break;
        }
        return false;
    }

    deltaTime = 0.0f;
    lastFrame = 0.0f;
    return true;
}


bool Scene::processEvent()
{
    bool done = false;
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        done = true;
                        break;
                    case SDLK_z:
                        camera.processKeys(Camera::Movement::FORWARD,
                                           deltaTime);
                        break;
                    case SDLK_s:
                        camera.processKeys(Camera::Movement::BACKWARD,
                                           deltaTime);
                        break;
                    case SDLK_q:
                        camera.processKeys(Camera::Movement::LEFT,
                                           deltaTime);
                        break;
                    case SDLK_d:
                        camera.processKeys(Camera::Movement::RIGHT,
                                           deltaTime);
                        break;
                    case SDLK_SPACE:
                        camera.processKeys(Camera::Movement::UP,
                                           deltaTime);
                        break;
                    case SDLK_LCTRL:
                        camera.processKeys(Camera::Movement::DOWN,
                                           deltaTime);
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        width = event.window.data1;
                        height = event.window.data2;
                        updateProjView();
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                camera.processMotion(event.motion.xrel, event.motion.yrel);
                break;
        }
        if (done)
            break;
    }
    return done;
}
