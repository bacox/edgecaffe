if (NOT HAVE_PYTHON)
    message(STATUS "Python interface is disabled or not all required dependencies found. Building without it...")
    return()
endif ()

file(GLOB_RECURSE python_caffe_srcs ${PROJECT_SOURCE_DIR}/python/caffe/*.cpp)
file(GLOB_RECURSE python_edgecaffe_srcs ${PROJECT_SOURCE_DIR}/python/edgecaffe/*.cpp)

add_library(py_edge_caffe SHARED ${python_edgecaffe_srcs})
add_dependencies(py_edge_caffe EdgeCaffeCore)

set_target_properties(py_edge_caffe PROPERTIES PREFIX "" OUTPUT_NAME "_edge_caffe")
target_include_directories(py_edge_caffe PUBLIC
        ${PYTHON_INCLUDE_DIRS_GLOBAL}
        ${NUMPY_INCLUDE_DIR_GLOBAL}
        ${CMAKE_SOURCE_DIR}/Core/include
        )
target_link_libraries(py_edge_caffe PUBLIC
        ${PYTHON_LIBRARIES_GLOBAL}
        EdgeCaffeCore
        #        INTERFACE
        ${Caffe_LINK_GLOBAL}
        "-lboost_python3 -lboost_numpy3"
        )


if (UNIX OR APPLE)
    set(__linkname "${PROJECT_SOURCE_DIR}/python/edgecaffe/_edge_caffe.so")
    add_custom_command(TARGET py_edge_caffe POST_BUILD
            COMMAND ln -sf $<TARGET_LINKER_FILE:py_edge_caffe> "${__linkname}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/python/caffe/proto
            COMMAND touch ${PROJECT_SOURCE_DIR}/python/caffe/proto/__init__.py
            COMMAND cp ${proto_gen_folder_GLOBAL}/*.py ${PROJECT_SOURCE_DIR}/python/caffe/proto/
            COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/python/_edge_caffe${Caffe_POSTFIX}.so")
endif ()

add_library(pycaffe SHARED ${python_caffe_srcs})
set_target_properties(pycaffe PROPERTIES PREFIX "" OUTPUT_NAME "_caffe")
target_include_directories(pycaffe PUBLIC ${PYTHON_INCLUDE_DIRS_GLOBAL} ${NUMPY_INCLUDE_DIR_GLOBAL})
target_link_libraries(pycaffe PUBLIC ${Caffe_LINK_GLOBAL} ${PYTHON_LIBRARIES_GLOBAL})

if (UNIX OR APPLE)
    set(__linkname "${PROJECT_SOURCE_DIR}/python/caffe/_caffe.so")
    add_custom_command(TARGET pycaffe POST_BUILD
            COMMAND ln -sf $<TARGET_LINKER_FILE:pycaffe> "${__linkname}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/python/caffe/proto
            COMMAND touch ${PROJECT_SOURCE_DIR}/python/caffe/proto/__init__.py
            COMMAND cp ${proto_gen_folder_GLOBAL}/*.py ${PROJECT_SOURCE_DIR}/python/caffe/proto/
            COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/python/_caffe${Caffe_POSTFIX}.so")
endif ()
