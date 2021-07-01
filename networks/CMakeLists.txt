cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Make sure the build type is set to Debug if not set upfront with 'cmake -DCMAKE_BUILD_TYPE=Debug ..'
set(default_build_type "Debug")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()
message(STATUS "CMAKE BUILD TYPE IS ${CMAKE_BUILD_TYPE}")

# Enable this line to include the memcheck in the build
#add_definitions ( -DMEMORY_CHECK_ON)

# Make sure that the install path is set to /opt/edgecaffe
SET(CMAKE_INSTALL_PREFIX /opt/edgecaffe)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# ---[ Main Target
project(EdgeCaffe_Networks C CXX)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMPONENT EdgeCaffeNetworks
        DESTINATION /opt/edgecaffe
        FILES_MATCHING
        PATTERN "*"
)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#####################################################
# ---[ Configuration for Debian package generater   #
# To generate an .deb install packages run: 'cpack' #
#####################################################
#SET(CPACK_GENERATOR "DEB")
#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "bacox") #required
#set(CPACK_COMPONENTS_ALL EdgeCaffeNetworks)
#set(CPACK_SET_DESTDIR true)
#set(CPACK_INSTALL_PREFIX /opt/edgecaffe)
#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
#set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
#set(CPACK_DEB_COMPONENT_INSTALL ON)
#set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DNN Inference for Edge Devices")
#SET(CPACK_PACKAGE_VENDOR "Bart Cox")
#SET(CPACK_PACKAGE_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR})
#SET(CPACK_PACKAGE_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR})
#SET(CPACK_PACKAGE_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH})
#
#INCLUDE(CPack)
