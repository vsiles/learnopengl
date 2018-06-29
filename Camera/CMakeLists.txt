project(Camera)
add_executable(Camera src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c
    src/texture.cpp)
target_link_libraries(Camera ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS Camera RUNTIME DESTINATION ${BIN_DIR})
