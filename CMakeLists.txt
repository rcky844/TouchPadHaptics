# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name and set the language to C++ (CXX)
project(MyClangProject LANGUAGES CXX)

# Set the C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required Qt components (e.g., Core, Widgets, Gui)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Create an executable named 'touchpad_haptics'
add_executable(touchpad_haptics main.cpp)
target_link_libraries(touchpad_haptics PRIVATE Qt6::Widgets)
