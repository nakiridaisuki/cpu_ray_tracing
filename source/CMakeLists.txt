set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp)

add_executable(CPURayTracing ${source})

target_include_directories(CPURayTracing PUBLIC include)
target_compile_definitions(CPURayTracing PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_FORCE_LEFT_HANDED)
target_link_libraries(CPURayTracing glm rapidobj)