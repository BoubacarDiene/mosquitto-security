##
#
# \file (src) CMakeLists.txt
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   March 2020
#
# \brief  CMakeLists.txt to prepare necessary things for sample
#         code to be built
#
##

#################################################################
#                       Compilation options                     #
#################################################################

# Export full path to etc/ directory
# Used in .c and .conf files
include(GNUInstallDirs)
set(BUILD_ETC_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}")

# Common flags
# See https://linux.die.net/man/1/gcc
#     https://security.stackexchange.com/questions/24444/what-is\
#      -the-most-hardened-set-of-options-for-gcc-compiling-c-c
list(APPEND LDFLAGS_OPTIONS -pthread -lmosquitto)
list(APPEND CFLAGS_OPTIONS -DBUILD_ETC_DIR="${BUILD_ETC_DIR}"
                           -std=c99 -D_GNU_SOURCE
                           -Wall -Wextra -Werror -Wconversion -Wsign-conversion
                           -Wuninitialized -Wparentheses -Winit-self -Wcomment
                           -Wstrict-prototypes -Wmissing-prototypes -Wshadow
                           -Wno-unused-result)

# Additional flags can be added depending on BUILD_TYPE
# environment variable
#
# -DBUILD_TYPE="asan secu" (at least one of these strings)
#   If not set => Release build
#   Otherwise => Debug build. Also,
#     asan => Add address sanitizer flags
#     secu => Add security flags
if(NOT BUILD_TYPE)
    list(APPEND CFLAGS_OPTIONS -s)
else()
    string(REPLACE " " ";" TYPES ${BUILD_TYPE})
    foreach(type ${TYPES})
        if(type STREQUAL "asan")
            # TODO
            # - lasan should normally not be needed (-fsanitize=address should be sufficient)
            # - PREPEND is used to avoid error "ASan runtime does not come first in initial library list"
            list(PREPEND LDFLAGS_OPTIONS -lasan)
            list(APPEND CFLAGS_OPTIONS -fsanitize=address -fno-omit-frame-pointer)
        elseif(type STREQUAL "secu")
            list(APPEND LDFLAGS_OPTIONS -Wl,-z,now -Wl,-z,relro -Wl,-z,noexecstack)
            list(APPEND CFLAGS_OPTIONS -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security
                                       -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4
                                       -pie -fPIE -ftrapv)
        endif()
    endforeach(type)
endif()

# Adds options to the COMPILE_OPTIONS directory property.
# These options are used when compiling targets from the current
# directory and below. COMPILE_OPTIONS is the list of options to
# pass to the compiler
#
# add_compile_options(${CFLAGS_OPTIONS})
set(CFLAGS_OPTIONS ${CFLAGS_OPTIONS})

# Add options to the link step for executable, shared library or
# module library targets in the current directory and below that
# are added after this command is invoked
#
# add_link_options(${LDFLAGS_OPTIONS})
set(LDFLAGS_OPTIONS ${LDFLAGS_OPTIONS})

# Add the given directories to those the compiler uses to search
# for include files. Relative paths are interpreted as relative
# to the current source directory
#
# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#################################################################
#                           Macros                              #
#################################################################

# List all subdirectories inside provided currentDir
macro(list_subdirectories result currentDir)
    file(GLOB children RELATIVE ${currentDir} ${currentDir}/*)
    set(dirList "")
    foreach(child ${children})
        if(IS_DIRECTORY ${currentDir}/${child})
            list(APPEND dirList ${child})
        endif()
    endforeach()
    set(${result} ${dirList})
endmacro()

# Generate executable from sourceFile and install it to outdir
macro(executable_from_source sourceFile destinationDirName executableName)
    # Generate a random number
    # This project generates several executables with the same name (publisher
    # or subscriber). add_executable() fails if used twice with the same name.
    # The executable will be renamed when installing it
    string(RANDOM randomNumber)

    # Create an exectuable from the source file
    add_executable(${executableName}-${randomNumber} ${sourceFile})

    # Compilation options
    target_compile_options(${executableName}-${randomNumber} PRIVATE ${CFLAGS_OPTIONS})
    target_include_directories(${executableName}-${randomNumber} PRIVATE ${HEADERS_PATH})
    target_link_libraries(${executableName}-${randomNumber} ${LDFLAGS_OPTIONS})

    # Install exectuable to bin directory
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${executableName}-${randomNumber}
            DESTINATION ${CMAKE_INSTALL_BINDIR}/${destinationDirName}
            RENAME ${executableName})
endmacro()

#################################################################
#                       Subdirectories                          #
#################################################################

# Get list of directories inside src/ directory
list_subdirectories(subdirectories ${CMAKE_CURRENT_SOURCE_DIR})

# Add all subdirectories that contain a CMakeLists.txt file
foreach(subdirectory ${subdirectories})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt)
        message("Add source directory: ${subdirectory}")
        add_subdirectory(${subdirectory})
    endif()
endforeach()
