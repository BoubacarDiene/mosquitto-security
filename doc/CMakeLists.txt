##
#
# \file (doc) CMakeLists.txt
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   March 2020
#
# \brief  CMakeLists.txt to generate and install documentation
#
# \note   "doxygen -g doc/Doxyfile.in" has generated Doxyfile.in
#         whose fields that are expected to be filled in by cmake
#         are set with cmake's variables
#
##

#################################################################
#                        Dependencies                           #
#################################################################

# Make sure doxygen is installed
find_package(Doxygen REQUIRED)

# Replace cmake's variables in Doxyfile.in
configure_file(Doxyfile.in Doxyfile)

#################################################################
#                         Variables                             #
#################################################################

set(DOXYGEN_INPUT ${CMAKE_BINARY_DIR}/doc/Doxyfile)
set(DOXYGEN_OUTPUT ${APIDOC_DIR}/html/index.html)

#################################################################
#                          Targets                              #
#################################################################

# Define "doc" target
# "make doc" or make (due to ALL) can be used to generate
# documentation
add_custom_target(doc ALL
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT} > /dev/null 2>&1
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
)

#################################################################
#                        Installation                           #
#################################################################

# Copy documentation to output directory when "make install" is
# called.
#
# From https://cmake.org/cmake/help/latest/command/install.html:
#   When TYPE is specified, a destination will then be set
#   automatically by taking the corresponding variable from
#   GNUInstallDirs, or by using a built-in default if that variable
#   is not defined
install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html
        TYPE DOC)
