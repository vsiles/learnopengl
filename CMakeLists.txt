cmake_minimum_required(VERSION 3.5)
project(opengl)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${opengl_SOURCE_DIR}/cmake")
# Set an output directory for our binaries
set(BIN_DIR ${opengl_SOURCE_DIR}/bin)

# Use c++11
set (CMAKE_CXX_STANDARD 11)

# Bump up warning levels appropriately for clang, gcc & msvc
# Also set debug/optimization flags depending on the build type.
# IDE users choose this when selecting the build mode in their IDE
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

# Vim YouCompleteMe stuff
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  execute_process (COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
endif()

# Look up SDL2 and add the include directory to our include path
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# Look up SDL2_Image
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

# Look up SDL2_ttf
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})

# Look up ZLIB
find_package(ZLIB REQUIRED)
# ZLIB includes are in the default compiler path

# Look up Opengl & alike
find_package(OpenGL REQUIRED)

# Local includes
include_directories(${opengl_SOURCE_DIR}/include)

# First test program
add_subdirectory(FirstWindow)
add_subdirectory(ShaderTest)
add_subdirectory(Texture)
