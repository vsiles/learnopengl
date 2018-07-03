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

SDL_Surface *Texture::flip(SDL_Surface *surf)
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
    /* Gather surface information */
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
        /* Should be deadcode by now, but won't hurt */
        cerr << "Unsupported BPP: " << (int)surf->format->BytesPerPixel;
        cerr << endl;
        return false;
    }

    /* Texture is validated, time to OpenGL it */
    if (glIsTexture(id))
        glDeleteTextures(1, &id);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surf->w, surf->h, 0, format,
                 GL_UNSIGNED_BYTE, surf->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

SDL_Surface *Texture::prepare(SDL_Surface *surf)
{
    Uint8 bpp = surf->format->BytesPerPixel;
    if ((bpp == 3) || (bpp == 4)) {
        /* Nothing to convert */
    } else if (bpp != 1) {
        cerr << "Unsupported BPP: " << (int)surf->format->BytesPerPixel;
        cerr << endl;
        SDL_FreeSurface(surf);
        return nullptr;
    } else {
        /* Need to convert from 'grayscale/palette' to RGBA */
        /* FIXME: SDL_PIXELFORMAT_ABGR8888 for little endian
         * SDL_PIXELFORMAT_RGBA32 is an 'endianess free' alias,
         * but only available on SLD2 > 2.0.5
         */
        SDL_Surface *csurf;
        csurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_ABGR8888, 0);
        SDL_FreeSurface(surf);
        if (csurf == nullptr) {
            cerr << "Can't convert surface: " << SDL_GetError() << endl;
            return nullptr;
        }
        surf = csurf;
    }

    SDL_Surface *isurf = flip(surf);
    SDL_FreeSurface(surf);
    if (isurf == nullptr) {
        cerr << "Can't flip surface" << endl;
        return nullptr;
    }
    return isurf;
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
    surf = prepare(surf);
    if (surf == nullptr) {
        return false;
    }

    bool ok = init(surf);
    SDL_FreeSurface(surf);
    return ok;
}

bool Texture::init(const string &path)
{
    SDL_Surface *surf = IMG_Load(path.c_str());
    if (surf == nullptr) {
        cerr << "IMG_Load(" << path << ") failure: " << SDL_GetError() << endl;
        return false;
    }

    surf = prepare(surf);
    if (surf == nullptr)
        return false;

    bool ok = init(surf);
    SDL_FreeSurface(surf);
    return ok;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, id);
}
