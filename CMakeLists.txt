##
#
# \file Root CMakeLists.txt
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   March 2020
#
# \brief  CMakeLists.txt to check cmake version, set configs
#         that apply to all sub-CMakeLists.txt, ...
#
# mkdir build && cd build
# cmake .. -DCMAKE_INSTALL_PREFIX=./out -DBUILD_TYPE="asan secu"
# make VERBOSE=true
# make install
#
##

cmake_minimum_required(VERSION 3.10.2)

#################################################################
#                          Project                              #
#################################################################

# Name
project(mosquitto-security)

# Build type
#
# Additional compilation flags can be added by setting BUILD_TYPE
# environment variable
#
# -DBUILD_TYPE="asan secu" (at least one of these strings)
#   If not set => Release build
#   Otherwise => Debug build. Also,
#     asan => Add address sanitizer flags
#     secu => Add security flags
if(NOT BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

#################################################################
#                       Subdirectories                          #
#################################################################

# Directory containing source code
add_subdirectory(src)

# Directory containing Doxyfile.in and everything needed to
# generate and install documentation
add_subdirectory(doc)

# Directory containing the script that allows to generate SSL/TLS
# certificates for broker(s) and clients
add_subdirectory(scripts)
