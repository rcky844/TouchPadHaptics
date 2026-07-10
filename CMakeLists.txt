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

# Support install excutable
include(GNUInstallDirs)
install(TARGETS touchpad_haptics
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Packaging support via CPack
set(CPACK_PACKAGE_NAME "tpd-haptics-cntrl")
set(CPACK_PACKAGE_VENDOR "Ricky Cheung")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple application to control haptic touchpad intensity.")
set(CPACK_PACKAGE_VERSION "1.0")

set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ricky Cheung <rcheung844@gmail.com>")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_RELEASE "1")

# Include the CPack module (this must be at the very end of the file)
include(CPack)
