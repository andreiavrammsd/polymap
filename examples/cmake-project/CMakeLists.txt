cmake_minimum_required(VERSION 3.17)
project(cmake-project)
set(PROJECT_VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror --coverage -fsanitize=undefined")

add_executable(cmake-project src/main.cpp)
set_target_properties(cmake-project PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(FetchContent)
if (NOT channel_POPULATED)
    FetchContent_Declare(channel URL https://github.com/andreiavrammsd/polymap/archive/v0.3.1.zip)
    FetchContent_Populate(channel)
    include_directories(${channel_SOURCE_DIR}/include)
endif ()
