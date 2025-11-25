cmake_minimum_required(VERSION 3.28)
project(glengine VERSION 0.0.1 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
file(GLOB_RECURSE SOURCES_SGLF CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# Define the library
add_executable(glengine ${SOURCES_SGLF})

# Include directories
target_include_directories(glengine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/build/dep/include
)

# Precompiled headers
target_precompile_headers(glengine PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp
)

# Compilation options
target_compile_options(glengine PRIVATE -Wall -Wextra -Wpedantic -fmax-errors=1)

# Link dependencies
target_link_directories(glengine PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/build/dep/lib
)

target_link_libraries(glengine PUBLIC
	glfw
	GLEW
	freetype
	tinygltf
	stb_vorbis
	GL
	dl
	pthread
	m
)

