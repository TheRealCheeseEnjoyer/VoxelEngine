cmake_minimum_required(VERSION 3.27)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 23)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(VoxelEngine main.cpp
        src/glad.c
        main.cpp
        src/Shader.h
        src/InputManager.h
        src/stb_image.cpp
        src/stb_image.h
        src/Voxel.h
        src/World.h
        src/Chunk.h
        src/TextureManager.h
        src/Camera.h
        src/PerlinNoise.h
        src/PerlinNoise.h
        src/ThreadPool.h
        src/Player.h
)


target_link_libraries(VoxelEngine -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl)