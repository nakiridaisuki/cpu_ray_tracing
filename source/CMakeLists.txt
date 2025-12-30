set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp)

add_executable(CPURayTracing ${source})

target_include_directories(CPURayTracing PUBLIC include)
target_link_libraries(CPURayTracing glm)