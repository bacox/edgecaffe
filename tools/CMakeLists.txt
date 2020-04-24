cmake_minimum_required(VERSION 3.1.0)
project(ModelSplitter)
set(CMAKE_CXX_STANDARD 17)
# Don't add this line if you will try_compile with boost.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_definitions(-DBOOST_ALL_DYN_LINK)

add_executable(${PROJECT_NAME}
        ModelSplitter.cpp
        )
add_dependencies(${PROJECT_NAME} caffe)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_ORIG_CAFFE})
target_link_libraries(${PROJECT_NAME}
        Threads::Threads
        -Wl,--whole-archive
        caffe
        -Wl,--no-whole-archive
        stdc++fs
        )


add_executable(GenerateNetwork
        GenerateNetwork.cpp
        )
add_dependencies(GenerateNetwork caffe)

target_include_directories(GenerateNetwork PUBLIC ${INCLUDE_ORIG_CAFFE})
target_link_libraries(GenerateNetwork
        Threads::Threads
        -Wl,--whole-archive
        caffe
        -Wl,--no-whole-archive
        stdc++fs
        )

