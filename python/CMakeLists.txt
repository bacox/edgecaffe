if(NOT HAVE_PYTHON)
  message(STATUS "Python interface is disabled or not all required dependencies found. Building without it...")
  return()
endif()

file(GLOB_RECURSE python_srcs ${PROJECT_SOURCE_DIR}/python/*.cpp)

add_library(py_edge_caffe SHARED ${python_srcs})
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
#        "-lboost_numpy -lboost_python"
        "-lboost_python3 -lboost_numpy3"
        )


#message(STATUS "PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS_GLOBAL}")
#message(STATUS "NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_DIR_GLOBAL}")
message(STATUS "Caffe_LINK ${Caffe_LINK_GLOBAL}")
message(STATUS "PYTHON_LIBRARIES ${PYTHON_LIBRARIES_GLOBAL}")


if(UNIX OR APPLE)
    set(__linkname "${PROJECT_SOURCE_DIR}/python/caffe/_edge_caffe.so")
    add_custom_command(TARGET py_edge_caffe POST_BUILD
                       COMMAND ln -sf $<TARGET_LINKER_FILE:py_edge_caffe> "${__linkname}"
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/python/caffe/proto
                       COMMAND touch ${PROJECT_SOURCE_DIR}/python/caffe/proto/__init__.py
                       COMMAND cp ${proto_gen_folder_GLOBAL}/*.py ${PROJECT_SOURCE_DIR}/python/caffe/proto/
                       COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/python/_edge_caffe${Caffe_POSTFIX}.so")
endif()
#
## ---[ Install
## scripts
#file(GLOB python_files *.py requirements.txt)
#install(FILES ${python_files} DESTINATION python)
#
## module
#install(DIRECTORY caffe
#    DESTINATION python
#    FILES_MATCHING
#    PATTERN "*.py"
#    PATTERN "ilsvrc_2012_mean.npy"
#    PATTERN "test" EXCLUDE
#    )
#
## _caffe.so
#install(TARGETS pycaffe  DESTINATION python/caffe)
#
#
#
#
#add_library(pyedgecaffe SHARED ${python_srcs})
#caffe_default_properties(pyedgecaffe)
#set_target_properties(pyedgecaffe PROPERTIES PREFIX "" OUTPUT_NAME "_caffe3")
#target_include_directories(pyedgecaffe PUBLIC ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIR})
#target_link_libraries(pyedgecaffe PUBLIC ${Caffe_LINK} ${PYTHON_LIBRARIES})
#
#if(UNIX OR APPLE)
#    set(__linkname "${PROJECT_SOURCE_DIR}/python/caffe/_caffe3.so")
#    add_custom_command(TARGET pyedgecaffe POST_BUILD
#            COMMAND ln -sf $<TARGET_LINKER_FILE:pyedgecaffe> "${__linkname}"
#            COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/python/caffe/proto
#            COMMAND touch ${PROJECT_SOURCE_DIR}/python/caffe/proto/__init__.py
#            COMMAND cp ${proto_gen_folder}/*.py ${PROJECT_SOURCE_DIR}/python/caffe/proto/
#            COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/lib/_caffe${Caffe_POSTFIX}.so")
#endif()
#
### ---[ Install
### scripts
##file(GLOB python_files *.py requirements.txt)
##install(FILES ${python_files} DESTINATION python)
##
### module
##install(DIRECTORY caffe
##        DESTINATION python
##        FILES_MATCHING
##        PATTERN "*.py"
##        PATTERN "ilsvrc_2012_mean.npy"
##        PATTERN "test" EXCLUDE
##        )
#
## _caffe.so
#install(TARGETS pyedgecaffe  DESTINATION python/caffe)
#
