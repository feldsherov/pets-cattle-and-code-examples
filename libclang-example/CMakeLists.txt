cmake_minimum_required(VERSION 3.12)
project(PetsCattleExample VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard for the entire project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(lib STATIC span.cc)

# Create an executable 'myapp' from 'main.cc'
add_executable(example main.cc)

# Link the library 'mylib' with the executable 'myapp'
target_link_libraries(example PRIVATE lib)
