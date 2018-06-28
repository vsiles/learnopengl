project(Texture)
add_executable(Texture src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c
    src/texture.cpp)
target_link_libraries(Texture ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS Texture RUNTIME DESTINATION ${BIN_DIR})
