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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    this->width = width;
    this->height = height;

    window = SDL_CreateWindow(name.c_str(),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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

void Scene::run()
{
    GLfloat vertices[] = {
        /* Positions        Colors            Textures */
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    {
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                     GL_STATIC_DRAW);

    }

    /* 0 as in 'location = 0' in the vertex shader: position */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          nullptr);
    glEnableVertexAttribArray(0);
    /* 1 as in 'location = 1' in the vertex shader: color */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    /* 2 as in 'location = 2' in the vertex shader: texture coord */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    /* Doing after the VAO is unbound */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* uncomment to draw in wireframe mode */
    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    SDL_Event event;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

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
            }
            if (done)
                break;
        }
        if (done)
            break;

        glClear(GL_COLOR_BUFFER_BIT);
        /* FIXME: Should be done on resize & stuff, not here */
        glViewport(0, 0, width, height);

        shader.activate();
        glActiveTexture(GL_TEXTURE0);
        tex0.bind();
        glActiveTexture(GL_TEXTURE1);
        tex1.bind();

        shader.setInt("texture1", 0);
        shader.setInt("texture2", 1);

        glBindVertexArray(VAO);

        GLfloat time = SDL_GetTicks() / 1000.0f;
        {
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
            trans = glm::rotate(trans, time,glm::vec3(0.0f, 0.0f, 1.0f));
            shader.setMat4("transform", glm::value_ptr(trans));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        {
            GLfloat factor = sin(time);
            glm::mat4 trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
            trans = glm::scale(trans, glm::vec3(factor, factor, factor));
            shader.setMat4("transform", glm::value_ptr(trans));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
        SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

bool Scene::init(string &res_path)
{
    string vertex_shader = res_path + "shaders/texture.vert";
    string fragment_shader = res_path + "shaders/texture.frag";

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
