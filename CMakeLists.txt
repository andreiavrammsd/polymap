cmake_minimum_required(VERSION 3.17)
project(poly_map)
set(PROJECT_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage -fsanitize=undefined")

include_directories(include)

enable_testing()
add_subdirectory(tests)
