##
#
# \file (example 1) CMakeLists.txt
#
# \author Boubacar DIENE <boubacar.diene@gmail.com>
# \date   March 2020
#
# \brief  CMakeLists.txt to build and install example 2
#
##

#################################################################
#                       Source files                            #
#################################################################

file(GLOB sources *.c)

#################################################################
#                      Build / Install                          #
#################################################################

# Get current directory name
get_filename_component(currentDirectoryName
                       ${CMAKE_CURRENT_LIST_DIR} NAME)

foreach(source ${sources})
    # Remove the extension from the source's filename
    get_filename_component(executableName ${source} NAME_WE)

    # Generate and install executable
    executable_from_source(${source}
                           ${currentDirectoryName}
                           "${executableName}")

    # Replace cmake variables in mosquitto.conf and save output
    # file to ${CMAKE_CURRENT_BINARY_DIR}/mosquitto.conf
    configure_file(mosquitto.conf mosquitto.conf)

    # Install files to the etc directory and give read permission
    # to owner and group
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mosquitto.conf passwords.txt
            DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${currentDirectoryName}
            PERMISSIONS OWNER_READ GROUP_READ)
endforeach()
