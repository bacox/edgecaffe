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
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/bin")

# ---[ Install settings
option(INSTALL_NETWORKS "Install the networks found in the build folder" OFF)
option(INSTALL_RESOURCES "Install the input files found in the build folder" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# ---[ Main Target
project(EdgeCaffe C CXX)

# ---[ EdgeCaffe Include list
set(EDGECAFFE_INCLUDE "")
# ---[ EdgeCaffe Linking list
set(EDGECAFFE_LINK "")

# ---[ EdgeCaffe version
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "2")
set(EDGECAFFE_TARGET_VERSION "1.3.2" CACHE STRING "EdgeCaffe logical version")
set(EDGECAFFE_TARGET_SOVERSION "1.3.2" CACHE STRING "EdgeCaffe soname version")
add_definitions(-DEDGECAFFE_VERSION=${EDGECAFFE_TARGET_VERSION})

# ---[ Flags
if (UNIX OR APPLE)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -std=c++11 -O3 -g")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -Wall")
    endif()
endif ()

# Setting compiler flags
SET(LINK_FLAGS "-L/usr/local/lib/ -lprotobuf -pthread -lyaml-cpp -lpthread -lboost_system -Wl,--no-whole-archive")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(HAVE_PYTHON TRUE)

# ---[ Dependencies

find_package(PkgConfig REQUIRED)
# ---[ YAML-CPP
find_package(yaml-cpp REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
list(APPEND EDGECAFFE_LINK ${YAML_CPP_LINK_LIBRARIES})

# ---[ Google-Glog
include("${CMAKE_SOURCE_DIR}/caffe/cmake/Modules/FindGlog.cmake")
list(APPEND EDGECAFFE_INCLUDE ${GLOG_INCLUDE_DIR})
list(APPEND EDGECAFFE_LINK ${GLOG_LIBRARIES})

# ---[ OpenCv
find_package(OpenCV REQUIRED)
list(APPEND EDGECAFFE_LINK ${OpenCV_LIBRARIES})

# ---[ Protobuf
include(FindProtobuf)
find_package(Protobuf REQUIRED)
list(APPEND EDGECAFFE_LINK ${PROTOBUF_LIBRARY})

# ---[ Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND EDGECAFFE_LINK Threads::Threads)

# ---[ stdc++fs
list(APPEND EDGECAFFE_LINK stdc++fs)

macro(DEP_EXISTS DEP_NAME)
    if (${${DEP_NAME}_FOUND})
    else ()
        MESSAGE(SEND_ERROR "Cannot find library ${DEP_NAME}")
    endif ()
endmacro()

# ---[ Check if all dependencies are found
DEP_EXISTS(GLOG)
DEP_EXISTS(YAML_CPP)
DEP_EXISTS(OpenCV)
DEP_EXISTS(PROTOBUF)
DEP_EXISTS(THREADS)

# ---[ Make sure blas is set to open
set(BLAS "open")

add_subdirectory(caffe)
# Set path to caffe's original onclude files
set(INCLUDE_ORIG_CAFFE ${CMAKE_CURRENT_SOURCE_DIR}/caffe/include)
add_subdirectory(Core)
add_subdirectory(tools)
add_subdirectory(python)

macro(ADD_BINARY_TARGET TARGET_NAME)
    add_executable(${TARGET_NAME}
            ${TARGET_NAME}.cpp)
    add_dependencies(${TARGET_NAME}
            caffe
            EdgeCaffeCore
            )

    target_include_directories(${TARGET_NAME}
            PUBLIC
            Core/include
            ${INCLUDE_ORIG_CAFFE}
            thirdparty/cxxopts
            )

    set_target_properties( ${TARGET_NAME}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            )
    install(TARGETS ${TARGET_NAME}
            COMPONENT EdgeCaffeApp
            RUNTIME DESTINATION "/opt/edgecaffe/bin"
            LIBRARY DESTINATION "/opt/edgecaffe/lib"
            ARCHIVE DESTINATION "/opt/edgecaffe/lib"
            DESTINATION "${CMAKE_INSTALL_PREFIX}"
            )
            if (${BUILD_SHARED_LIBS})
#        Make sure not the whole archive is included in linking
        target_link_libraries(${TARGET_NAME}
                EdgeCaffeCore
                caffe
                ${EDGECAFFE_LINK}
                )
    else ()
#        The whole archive of caffe needs to be set for static linking
        target_link_libraries(${TARGET_NAME}
                EdgeCaffeCore
                -Wl,--whole-archive
                caffe
                -Wl,--no-whole-archive
                ${EDGECAFFE_LINK}
                )
    endif ()
endmacro()

# ---[ Set all the targets to be build
#ADD_BINARY_TARGET(ScheduledPipeline)
#ADD_BINARY_TARGET(RunPipeline)
#ADD_BINARY_TARGET(Exp_const_arrivals)
#ADD_BINARY_TARGET(Exp_poisson_arrival_baseline)
ADD_BINARY_TARGET(Exp_poisson_arrival_rv_nf)

# ---[ Copy resourcess
file(COPY ${CMAKE_SOURCE_DIR}/networks DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/scripts DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/experiments DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/thermal DESTINATION ${CMAKE_BINARY_DIR})

# ---[ Set a target to build multiple targets at the same time
add_custom_target(EdgeCaffe_build_all
        COMMAND
        ${CMAKE_COMMAND} -E echo Finished building build_all targets
        VERBATIM
        )

add_dependencies(EdgeCaffe_build_all
        caffe
        EdgeCaffeCore
#        RunPipeline
#        Exp_const_arrivals
        ModelSplitter
        __pycaffe
        pycaffe
        )

install(TARGETS caffe __pycaffe pycaffe caffeproto
        COMPONENT EdgeCaffeApp
        RUNTIME DESTINATION "/opt/edgecaffe/bin"
        LIBRARY DESTINATION "/opt/edgecaffe/lib"
        ARCHIVE DESTINATION "/opt/edgecaffe/lib"
        )

##############################################################
# Configuration for installing EdgeCaffe                     #
# Run 'sudo make install' to install on the current system   #
#                                                            #
# To fix the folder permissions of the installed EdgeCaffe   #
# run in the terminal:                                       #
#   'cd /opt && sudo chown -R $USER:$USER edgecaffe'         #
##############################################################


#install(TARGETS RunPipeline caffe ModelSplitter EdgeCaffeCore Exp_const_arrivals
#        COMPONENT EdgeCaffeApp
#        RUNTIME DESTINATION "/opt/edgecaffe/bin"
#        LIBRARY DESTINATION "/opt/edgecaffe/lib"
#        ARCHIVE DESTINATION "/opt/edgecaffe/lib"
#        DESTINATION "${CMAKE_INSTALL_PREFIX}"
#        )

#install(FILES ${CMAKE_BINARY_DIR}/caffe/lib/libcaffe-d.so.1.0.0
#        COMPONENT EdgeCaffeApp
#        DESTINATION "${CMAKE_INSTALL_PREFIX}"
#        )

INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/config DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)
#INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)
if (${INSTALL_RESOURCES})
#    INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_INSTALL_PREFIX})
    INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)
endif ()
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)

# Install experiment configs
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/experiments DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)


if (${INSTALL_NETWORKS})
#    INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/networks DESTINATION /opt/edgecaffe  COMPONENT EdgeCaffeApp)
endif ()
MESSAGE(STATUS "CMAKE INSTALL PREFIX '${CMAKE_INSTALL_PREFIX}'")
#INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT EdgeCaffeApp)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/thermal DESTINATION /opt/edgecaffe COMPONENT EdgeCaffeApp)
install(FILES ${CMAKE_SOURCE_DIR}/thermal/temperature.service DESTINATION /etc/systemd/system/ COMPONENT EdgeCaffeApp)
install(FILES ${CMAKE_SOURCE_DIR}/thermal/temperature.timer DESTINATION /etc/systemd/system/ COMPONENT EdgeCaffeApp)
# Make sure /opt/analysis exists
INSTALL(CODE "EXECUTE_PROCESS(COMMAND mkdir -p /opt/analysis)" COMPONENT EdgeCaffeApp)

install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/networks
        COMPONENT EdgeCaffeApp
        DESTINATION /opt/edgecaffe
        FILES_MATCHING
        PATTERN "*"
        PATTERN "*.caffemodel" EXCLUDE
        PATTERN "*_partials" EXCLUDE
)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#####################################################
# ---[ Configuration for Debian package generater   #
# To generate an .deb install packages run: 'cpack' #
#####################################################
SET(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "bacox") #required
set(CPACK_COMPONENTS_ALL EdgeCaffeApp)
set(CPACK_SET_DESTDIR true)
set(CPACK_INSTALL_PREFIX /opt/edgecaffe)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DNN Inference for Edge Devices")
SET(CPACK_PACKAGE_VENDOR "Bart Cox")

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "systemctl daemon-reload;mkdir -p /opt/analysis/thermal;")
INCLUDE(CPack)

MESSAGE(STATUS "CXX flags ${CMAKE_CXX_FLAGS}")
