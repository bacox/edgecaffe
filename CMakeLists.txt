cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
# Don't add this line if you will try_compile with boost.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---[ Product
project(EdgeCaffe C CXX)

# ---[ EdgeCaffe version
set(EDGECAFFE_TARGET_VERSION "1.0.0" CACHE STRING "EdgeCaffe logical version")
set(EDGECAFFE_TARGET_SOVERSION "1.0.0" CACHE STRING "EdgeCaffe soname version")
add_definitions(-DEDGECAFFE_VERSION=${EDGECAFFE_TARGET_VERSION})

# ---[ Flags
if(UNIX OR APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")
endif()

SET(LINK_FLAGS "-L/usr/local/lib/ -lprotobuf -pthread -lpthread -lboost_system -Wl,--whole-archive ${CMAKE_BINARY_DIR}/caffe/lib/libcaffe-d.a -Wl,--no-whole-archive")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# ---[ Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)


find_package (Eigen3 3.3 REQUIRED)
find_package(OpenCV REQUIRED )
include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(/usr/include/glog)
set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(HAVE_PYTHON TRUE)
add_subdirectory(caffe)
set(INCLUDE_ORIG_CAFFE ${CMAKE_CURRENT_SOURCE_DIR}/caffe/include)
add_subdirectory(Core)
add_subdirectory(tools)
add_subdirectory(python)

add_executable(ScheduledPipeline
        ScheduledPipeline.cpp)
add_dependencies(ScheduledPipeline
        EdgeCaffeCore
        )

target_include_directories(ScheduledPipeline
        PUBLIC
        Core/include
        ${INCLUDE_ORIG_CAFFE}
        )
target_link_libraries(ScheduledPipeline
        caffe
        ${PROTOBUF_LIBRARY}
        # Use pixed paths for now
        /usr/local/lib/libprotobuf.so
        ${OpenCV_LIBRARIES}
        ${Boost_LIBRARIES}
        # Use pixed paths for now
        /usr/lib/x86_64-linux-gnu/libglog.so
        Threads::Threads
        stdc++fs
        ${YAML_CPP_LINK_LIBRARIES}
        EdgeCaffeCore
        )


add_executable(RunPipeline
        RunPipeline.cpp)
add_dependencies(RunPipeline
        EdgeCaffeCore
        )

target_include_directories(RunPipeline
        PUBLIC
        Core/include
        ${INCLUDE_ORIG_CAFFE}
        )
target_link_libraries(RunPipeline
        caffe
        ${PROTOBUF_LIBRARY}
        # Use pixed paths for now
        /usr/local/lib/libprotobuf.so
        ${OpenCV_LIBRARIES}
        ${Boost_LIBRARIES}
        # Use pixed paths for now
        /usr/lib/x86_64-linux-gnu/libglog.so
        Threads::Threads
        stdc++fs
        ${YAML_CPP_LINK_LIBRARIES}
        EdgeCaffeCore
        )

# Copy resourcess
file(COPY ${CMAKE_SOURCE_DIR}/networks DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "YAML include ${YAML_CPP_INCLUDEDIR}")
message(STATUS "YAML link ${YAML_CPP_LINK_LIBRARIES}")
message(STATUS "YAML link 2 ${YAML_CPP_LIBRARIES}")

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