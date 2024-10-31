cmake_minimum_required (VERSION 3.1)
project(spine_reader)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(spine_include_dir ${PROJECT_SOURCE_DIR}/spine-cpp/include)
include_directories(${spine_include_dir})

file(GLOB MAIN_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB MAIN_HEAD ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB spine_src ${PROJECT_SOURCE_DIR}/spine-cpp/src/spine/*.cpp)

add_executable(${PROJECT_NAME} ${MAIN_SRC} ${MAIN_HEAD} ${spine_src})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()
