cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---[ Main Target
project(EdgeCaffe C CXX)

# ---[ EdgeCaffe Include list
set(EDGECAFFE_INCLUDE "")
# ---[ EdgeCaffe Linking list
set(EDGECAFFE_LINK "")

# ---[ EdgeCaffe version
set(EDGECAFFE_TARGET_VERSION "1.0.0" CACHE STRING "EdgeCaffe logical version")
set(EDGECAFFE_TARGET_SOVERSION "1.0.0" CACHE STRING "EdgeCaffe soname version")
add_definitions(-DEDGECAFFE_VERSION=${EDGECAFFE_TARGET_VERSION})

# ---[ Flags
if (UNIX OR APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")
endif ()

SET(LINK_FLAGS "-L/usr/local/lib/ -lprotobuf -pthread -lpthread -lboost_system -Wl,--whole-archive ${CMAKE_BINARY_DIR}/caffe/lib/libcaffe-d.a -Wl,--no-whole-archive")
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
list(APPEND EDGECAFFE_INCLUDE ${GLOG_INCLUDE_DIR})

# ---[ Google-Glog
include("./caffe/cmake/Modules/FindGlog.cmake")
list(APPEND EDGECAFFE_LINK ${GLOG_LIBRARIES})

## ---[ Eigen
#find_package (Eigen3 3.3 REQUIRED)
#list(APPEND EDGECAFFE_LINK ${YAML_CPP_LINK_LIBRARIES})

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
        #        message(STATUS "LIB STATUS ${DEP_NAME} is ${${DEP_NAME}_FOUND}")
    else ()
        MESSAGE(SEND_ERROR "Cannot find library ${DEP_NAME}")
    endif ()
endmacro()
# Check if all dependencies are found
DEP_EXISTS(GLOG)
DEP_EXISTS(YAML_CPP)
DEP_EXISTS(OpenCV)
DEP_EXISTS(PROTOBUF)
DEP_EXISTS(THREADS)


add_subdirectory(caffe)
set(INCLUDE_ORIG_CAFFE ${CMAKE_CURRENT_SOURCE_DIR}/caffe/include)
add_subdirectory(Core)
add_subdirectory(tools)
add_subdirectory(python)

macro(ADD_BINARY_TARGET TARGET_NAME)
    add_executable(${TARGET_NAME}
            ${TARGET_NAME}.cpp)
    add_dependencies(${TARGET_NAME}
            EdgeCaffeCore
            )

    target_include_directories(${TARGET_NAME}
            PUBLIC
            Core/include
            ${INCLUDE_ORIG_CAFFE}
            )
    target_link_libraries(${TARGET_NAME}
            caffe
            ${EDGECAFFE_LINK}
            EdgeCaffeCore
            )
endmacro()

ADD_BINARY_TARGET(ScheduledPipeline)
ADD_BINARY_TARGET(RunPipeline)
#ADD_BINARY_TARGET(ArrivalList)

# Copy resourcess
file(COPY ${CMAKE_SOURCE_DIR}/networks DESTINATION ${CMAKE_BINARY_DIR})

#message(STATUS "YAML include ${YAML_CPP_INCLUDEDIR}")
#message(STATUS "YAML link ${YAML_CPP_LINK_LIBRARIES}")
#message(STATUS "YAML link 2 ${YAML_CPP_LIBRARIES}")

add_custom_target(EdgeCaffe_build_all
        COMMAND
        ${CMAKE_COMMAND} -E echo Finished building build_all targets
        VERBATIM
        )

add_dependencies(EdgeCaffe_build_all
        ModelSplitter
        ScheduledPipeline
        EdgeCaffeCore
        __pycaffe
        pycaffe
        )
