#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include "main.hpp"

namespace vinz {
    class Texture {
        private:
            GLuint id;
            bool init(SDL_Surface *surf);
            static SDL_Surface *flip(SDL_Surface *surf);
            static SDL_Surface *convert(SDL_Surface *surf);
            SDL_Surface *prepare(SDL_Surface *surf);

        public:
            Texture(): id(0) {}
            ~Texture();
            Texture(Texture const &) = delete;
            Texture(Texture &&) = default;

            Texture& operator=(Texture const &) = delete;
            Texture& operator=(Texture &&) = default;

            bool init(const std::string &path);
            bool init(const std::vector<unsigned char> &data, size_t bytes);

            void bind();
    };
};
