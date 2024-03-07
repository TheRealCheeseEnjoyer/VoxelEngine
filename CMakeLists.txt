cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall)

add_executable(VoxelEngine main.cpp
        src/glad.c
        main.cpp
        src/Shader.h
        src/InputManager.h
        src/stb_image.cpp
        src/stb_image.h
)


target_link_libraries(VoxelEngine -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl)