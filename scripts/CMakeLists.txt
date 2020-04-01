##
#
# \file (scripts) CMakeLists.txt
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   March 2020
#
# \brief  CMakeLists.txt to generate and install SSL/TLS certificates
#
##

#################################################################
#                         Variables                             #
#################################################################

set(GENERATE_CERTIFICATES_COMMAND ${CMAKE_SOURCE_DIR}/scripts/generate-certificates.sh)
set(CERTIFICATES_OUTDIR ${CMAKE_BINARY_DIR})

#################################################################
#                          Targets                              #
#################################################################

# Define "certificates" target
# "make certificates" or make (due to ALL) can be used to
# generate certificates
add_custom_target(certificates ALL
  COMMAND ${CMAKE_COMMAND} -E echo_append "Generate SSL/TLS certificates..."
  COMMAND ${GENERATE_CERTIFICATES_COMMAND} ${CERTIFICATES_OUTDIR} > /dev/null 2>&1
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
)

#################################################################
#                        Installation                           #
#################################################################

# Copy certificates to output directory when "make install" is
# called.
#
# From https://cmake.org/cmake/help/latest/command/install.html:
#   When TYPE is specified, a destination will then be set
#   automatically by taking the corresponding variable from
#   GNUInstallDirs, or by using a built-in default if that variable
#   is not defined
install(DIRECTORY ${CMAKE_BINARY_DIR}/certificates
        TYPE SYSCONF)
