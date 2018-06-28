#include <iostream>
#include <string>
#include <cstdlib>
#include "scene.hpp"

using namespace std;
using namespace vinz;

#define WIDTH   800
#define HEIGHT  600

static bool get_resource_path(string &path)
{
    char *basePath = SDL_GetBasePath();
    if (basePath == nullptr) {
        cerr << "SDL_GetBasePath() failed: " << SDL_GetError() << endl;
        return false;
    }

    path = basePath;
    SDL_free(basePath);

    /* We replace the last bin/ with res/ to get the the resource path */
    size_t pos = path.rfind("bin");
    path = path.substr(0, pos) + "resources/";
    return true;
}

int main(void)
{
    int ret = EXIT_FAILURE;
    try {
        Scene scene("Hello GL", WIDTH, HEIGHT);
        string path;
        if (!get_resource_path(path))
            return EXIT_FAILURE;

        if (!scene.init(path))
            return EXIT_FAILURE;
        scene.run();
        ret = EXIT_SUCCESS;
    } catch (SceneFailure &excp) {
        switch (excp.cause) {
            case SceneFailure::Cause::Init:
                cerr << "SDL_Init() failed: " << excp.str << endl;
                break;
            case SceneFailure::Cause::Window:
                cerr << "SDL_CreateWindow() failed: " << excp.str << endl;
                break;
            case SceneFailure::Cause::Context:
                cerr << "SDL_GL_CreateContext() failed: " << excp.str << endl;
                break;
            case SceneFailure::Cause::Extension:
                cerr << "gl3wInit() failed: " << excp.str << endl;
                break;
        }
    }
    return ret;
}
