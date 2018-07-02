#pragma once

#include "main.hpp"
#include "glm/vec3.hpp"
#include "texture.hpp"

namespace vinz {
    class Cube {
        private:
            // TODO: maybe add color and texture in the future ?
            // TODO: add support for model position/rotation/scale in this class
            //       or in a parent one.
            //       For the moment, this is done "outside", in scene.cpp
            GLuint VAO, VBO;
        public:
            Cube(): VAO(0), VBO(0) {}
            Cube(const Cube &c) = default;
            Cube(Cube &&c) = default;

            Cube& operator=(Cube const &) = default;
            Cube& operator=(Cube &&) = default;

            ~Cube();

            void init();
            void render();
    };
};
