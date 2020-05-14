cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)
# Don't add this line if you will try_compile with boost.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(ModelSplitter C CXX)

set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PythonLibs 3.6)
add_definitions(-DBOOST_ALL_DYN_LINK)

macro(LINK_TOOL_TARGET TARGET_NAME)
    if (${BUILD_SHARED_LIBS})
#        MESSAGE(STATUS "Building TOOL ${TARGET_NAME} in shared lib setting")
        #        Make sure not the whole archive is included in linking
        target_link_libraries(
                ${TARGET_NAME}
                Threads::Threads
                caffe
                EdgeCaffeCore
                stdc++fs
                ${YAML_CPP_LINK_LIBRARIES}
                ${PYTHON_LIBRARIES}
        )
    else ()
        #        The whole archive of caffe needs to be set for static linking
        target_link_libraries(${TARGET_NAME}
                Threads::Threads
                -Wl,--whole-archive
                caffe
                -Wl,--no-whole-archive
                EdgeCaffeCore
                stdc++fs
                ${YAML_CPP_LINK_LIBRARIES}
                ${PYTHON_LIBRARIES}
                )
    endif()
endmacro()

add_executable(${PROJECT_NAME}
        ModelSplitter.cpp
        )
add_dependencies(${PROJECT_NAME} caffe)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_ORIG_CAFFE})
LINK_TOOL_TARGET(${PROJECT_NAME})


add_executable(GenerateNetwork
        GenerateNetwork.cpp
        )
add_dependencies(GenerateNetwork caffe)

target_include_directories(GenerateNetwork PUBLIC ${INCLUDE_ORIG_CAFFE})
LINK_TOOL_TARGET(GenerateNetwork)

add_executable(ExtendNetworkDescription
        ExtendNetworkDescription.cpp
        )
add_dependencies(ExtendNetworkDescription caffe Threads::Threads)

target_include_directories(ExtendNetworkDescription PUBLIC ${INCLUDE_ORIG_CAFFE})
LINK_TOOL_TARGET(ExtendNetworkDescription)
