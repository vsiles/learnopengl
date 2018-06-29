#include <iostream>
/* #define GLM_FORCE_MESSAGES */
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "scene.hpp"

using namespace std;
using namespace vinz;

Scene::Scene(string name, int width, int height)
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
    GLfloat vertices[] = {
        /* Position           TexCoord */
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    {
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);
    }

    /* 0 as in 'location = 0' in the vertex shader: position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          nullptr);
    glEnableVertexAttribArray(0);
    /* 1 as in 'location = 1' in the vertex shader: texture coord */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    /* Doing after the VAO is unbound */
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* uncomment to draw in wireframe mode */
    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    SDL_Event event;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    static glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    /* glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 3.0f); */
    /* glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f); */
    /* glm::vec3 camDirection = glm::normalize(camPosition - camTarget); */

    /* glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); */
    /* glm::vec3 camRight = glm::normalize(glm::cross(up, camDirection)); */

    /* glm::vec3 camUp = glm::cross(camDirection, camRight); */

    while (true) {
        bool done = false;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            done = true;
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
            }
            if (done)
                break;
        }
        if (done)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();
        glActiveTexture(GL_TEXTURE0);
        tex0.bind();
        glActiveTexture(GL_TEXTURE1);
        tex1.bind();

        shader.setInt("texture1", 0);
        shader.setInt("texture2", 1);

        glBindVertexArray(VAO);

        /* Setup View matrix */
        /* glm::mat4 view = glm::lookAt(camPosition, camTarget, camUp); */
        GLfloat radius = 10.0f;
        GLfloat camX = sin(SDL_GetTicks() / 1000.0f) * radius;
        GLfloat camZ = cos(SDL_GetTicks() / 1000.0f) * radius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),
                                     glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));

        /* Projection matrix is updated when needed */
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));

        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

bool Scene::init(string &res_path)
{
    string vertex_shader = res_path + "shaders/coordsys.vert";
    string fragment_shader = res_path + "shaders/coordsys.frag";

    try {
        cerr << "Vertex shader compiling..." << endl;
        Shader vert(vertex_shader, Shader::Type::Vertex);
        cerr << "Fragment shader compiling..." << endl;
        Shader frag(fragment_shader, Shader::Type::Fragment);
        ShaderFailure cause;
        if (!shader.init(vert, frag, cause))
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

    cerr << "Loading container.jpg" << endl;
    if (!tex0.init(res_path + "images/container.jpg")) {
        cerr << "Can't load container.jpg" << endl;
        return false;
    }
    cerr << "Loading awesomeface.png" << endl;
    if (!tex1.init(res_path + "images/awesomeface.png")) {
        cerr << "Can't load awesomeface.png" << endl;
        return false;
    }

    return true;
}
