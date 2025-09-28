cmake_minimum_required(VERSION 3.5)
project(my6502 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_library(my6502)
target_sources(my6502 PRIVATE src/emu6502.cpp)

target_compile_features(my6502 PUBLIC cxx_std_17)
target_include_directories(my6502 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(external)
add_subdirectory(test)
