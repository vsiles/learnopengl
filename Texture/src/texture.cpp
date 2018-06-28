#include <fstream>
#include <iostream>
#include <SDL_image.h>
#include "texture.hpp"

using namespace vinz;
using namespace std;

Texture::~Texture()
{
    if (glIsTexture(id))
        glDeleteTextures(1, &id);
}

SDL_Surface* Texture::flip(SDL_Surface *surf)
{
    SDL_Surface *isurf;
    int w = surf->w;
    int h = surf->h;
    isurf = SDL_CreateRGBSurface(0, w, h, surf->format->BitsPerPixel,
                                 surf->format->Rmask, surf->format->Gmask,
                                 surf->format->Bmask, surf->format->Amask);

    if (isurf == nullptr) {
        cerr << "SDL_CreateRGBSurface() failure: " << SDL_GetError() << endl;
        return nullptr;
    }

    unsigned char *src = (unsigned char *)surf->pixels;
    unsigned char *dst = (unsigned char *)isurf->pixels;
    int length = w * surf->format->BytesPerPixel;

    for (int j = 0; j < surf->h; j++) {
        for( int i = 0; i < length; i++) {
            dst[(h - 1 - j) * length + i] = src[j * length + i];
        }
    }

    return isurf;
}

bool Texture::init(SDL_Surface *surf)
{
    if (glIsTexture(id))
        glDeleteTextures(1, &id);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum internalFormat, format;
    if (surf->format->BytesPerPixel == 3) {
        internalFormat = GL_RGB;
        if (surf->format->Rmask == 0xff)
            format = GL_RGB;
        else
            format = GL_BGR;
    } else if (surf->format->BytesPerPixel == 4) {
        internalFormat = GL_RGBA;
        if (surf->format->Rmask == 0xff)
            format = GL_RGBA;
        else
            format = GL_BGRA;
    } else {
        cerr << "Unsupported BPP: " << surf->format->BytesPerPixel << endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &id);
        return false;
    }


    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surf->w, surf->h, 0, format,
                 GL_UNSIGNED_BYTE, surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Texture::init(const vector<unsigned char> &data, size_t bytes)
{
    SDL_RWops *ro_ops = SDL_RWFromConstMem(&(data[0]), bytes);
    if (ro_ops == nullptr) {
        cerr << "SDL_RWFromConstMem() failure: " << SDL_GetError() << endl;
        return false;
    }
    SDL_Surface *surf = IMG_Load_RW(ro_ops, 0);
    SDL_FreeRW(ro_ops);

    if (surf == nullptr) {
        cerr << "IMG_Load_RW() failure: " << SDL_GetError() << endl;
        return false;
    }
    SDL_Surface *isurf = flip(surf);
    SDL_FreeSurface(surf);
    if (isurf == nullptr) {
        cerr << "Can't flip surface" << endl;
        return false;
    }

    bool ok = init(isurf);
    SDL_FreeSurface(isurf);
    return ok;
}

bool Texture::init(const string &path)
{
    SDL_Surface *surf = IMG_Load(path.c_str());
    if (surf == nullptr) {
        cerr << "IMG_Load(" << path << ") failure: " << SDL_GetError() << endl;
        return false;
    }

    SDL_Surface *isurf = flip(surf);
    SDL_FreeSurface(surf);
    if (isurf == nullptr) {
        cerr << "Can't flip surface" << endl;
        return false;
    }

    bool ok = init(isurf);
    SDL_FreeSurface(isurf);
    return ok;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, id);
}
