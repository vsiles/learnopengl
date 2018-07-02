project(LightBase)
add_executable(LightBase src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c
    src/texture.cpp src/camera.cpp src/cube.cpp)
target_link_libraries(LightBase ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS LightBase RUNTIME DESTINATION ${BIN_DIR})
