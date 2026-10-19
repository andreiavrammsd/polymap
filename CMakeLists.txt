cmake_minimum_required(VERSION 3.17)
project(poly_map)
set(PROJECT_VERSION 0.3.0)

add_library(msd_poly_map INTERFACE)
target_include_directories(msd_poly_map INTERFACE include)
target_compile_features(msd_poly_map INTERFACE cxx_std_17)

enable_testing()
add_subdirectory(tests)

add_subdirectory(examples)

install(DIRECTORY include/ DESTINATION include)
