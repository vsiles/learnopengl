project(CoordSys)
add_executable(CoordSys src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c
    src/texture.cpp)
target_link_libraries(CoordSys ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS CoordSys RUNTIME DESTINATION ${BIN_DIR})
