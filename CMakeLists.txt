cmake_minimum_required(VERSION 3.5)
project(main VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_FILES
#    main.cpp)
    main_6502.cpp
    main_6502.h)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
add_subdirectory(external)
add_subdirectory(my-6502-test)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
