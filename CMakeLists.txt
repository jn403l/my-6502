cmake_minimum_required(VERSION 3.5)
project(my-6502 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
add_subdirectory(external)
add_subdirectory(test)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main_6502.cpp src/emu6502.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
