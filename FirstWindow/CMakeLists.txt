project(FirstWindow)
add_executable(FirstWindow src/main.cpp src/scene.cpp src/shader.cpp src/gl3w.c)
target_link_libraries(FirstWindow ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY}
    ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

install(TARGETS FirstWindow RUNTIME DESTINATION ${BIN_DIR})
