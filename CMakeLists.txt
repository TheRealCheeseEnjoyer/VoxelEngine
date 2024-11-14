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
        include/Shader.h
        include/InputManager.h
        src/stb_image.cpp
        include/stb_image.h
        include/Voxel.h
        include/World.h
        include/Chunk.h
        include/TextureManager.h
        include/Camera.h
        include/PerlinNoise.h
        include/PerlinNoise.h
        include/ThreadPool.h
        include/Player.h
        include/AABB.h
        include/Collisions.h
        src/Camera.cpp
        src/Chunk.cpp
        src/Collisions.cpp
        src/Player.cpp
        src/Shader.cpp
        src/TextureManager.cpp
        src/ThreadPool.cpp
        src/World.cpp
)


target_link_libraries(VoxelEngine -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl)