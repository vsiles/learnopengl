#pragma once

#include <SDL.h>
#include <string>
#include <memory>
#include <vector>
#include "shader.hpp"
#include "texture.hpp"

struct SceneFailure {
    enum class Cause {
        Init, Window, Context, Extension
    };
    Cause cause;
    const char *str;
    SceneFailure(Cause cause, const char *str):cause(cause),str(str) {};
};

namespace vinz {
    class Scene {
        private:
            SDL_Window *window;
            SDL_GLContext glcontext;
            int width;
            int height;
            ShaderProgram shader;
            Texture tex0, tex1;

        public:
            Scene() = delete;
            Scene(std::string name, int width, int height);
            virtual ~Scene();

            /* Forbid copy or assignment */
            Scene(Scene const &) = delete;
            Scene(Scene &&) = delete;
            Scene& operator=(Scene const &) = delete;
            Scene& operator=(Scene &&) = delete;

            void run();
            bool init(std::string &res_path);
    };
};
