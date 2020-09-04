cmake_minimum_required(VERSION 3.15)

project("Course of Dimensionality")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

add_executable(course_dimensionality source/main.cpp)