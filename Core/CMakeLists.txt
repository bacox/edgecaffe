cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---[ Product
project(EdgeCaffeCore C CXX)

# collect files
file(GLOB_RECURSE hdrs ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if (${BUILD_SHARED_LIBS})
    add_library(${PROJECT_NAME} SHARED ${srcs})
else()
    add_library(${PROJECT_NAME} STATIC ${srcs})
endif ()

add_dependencies(${PROJECT_NAME} caffe)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

if (${BUILD_SHARED_LIBS})
    target_link_libraries(${PROJECT_NAME}
            caffe
            ${EDGECAFFE_LINK}
            )
else()
    target_link_libraries(${PROJECT_NAME}
            -Wl,--whole-archive
            caffe
            -Wl,--no-whole-archive
            ${EDGECAFFE_LINK}
            )
endif ()
