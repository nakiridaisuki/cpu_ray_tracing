cmake_minimum_required(VERSION 3.10)

project(
    CPURayTracing
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/rapidobj)
add_subdirectory(source)