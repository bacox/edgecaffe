cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---[ Product
project(EdgeCaffeCore C CXX)


add_executable(scheduler_benchmark scheduler.cpp)

target_include_directories(scheduler_benchmark
        PUBLIC
        ../include)

add_dependencies(scheduler_benchmark
        EdgeCaffeCore
        )

target_link_libraries(scheduler_benchmark
        EdgeCaffeCore
        )