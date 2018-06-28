project(ShaderTest)
add_executable(ShaderTest src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c)
target_link_libraries(ShaderTest ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS ShaderTest RUNTIME DESTINATION ${BIN_DIR})
