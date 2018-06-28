# learnopengl
some SDL2/OpenGL code based on [learnopengl](https://learnopengl.com), to learn
about new OpenGL design.

Depends on:
- C++
- [CMake](https://cmake.org/)
- [GL3W](https://github.com/skaslev/gl3w/)
- [GLM](https://glm.g-truc.net/0.9.9/index.html)
- [SDL 2.0](https://www.libsdl.org/)
- [SDL\_image 2.0](https://www.libsdl.org/projects/SDL_image/)

# Building information
```shell
$ cd /path/to/learnopengl
$ mkdir debug  # or release
$ cd debug     # or release
$ # for Windows ...
$ cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug # or Release
$ # or for Linux
$ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug  # or Release
$ make
$ make install
$ cd ../bin
$ # Enjoy !
```
