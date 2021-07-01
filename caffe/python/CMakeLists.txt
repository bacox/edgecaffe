if(NOT HAVE_PYTHON)
  message(STATUS "Python interface is disabled or not all required dependencies found. Building without it...")
  return()
endif()

if(NOT ${BUILD_SHARED_LIBS})
    message(STATUS "Cannot build python bindings with BUILD_SHARED_LIBS=off")
    return()
endif()


file(GLOB_RECURSE python_srcs ${PROJECT_SOURCE_DIR}/python/*.cpp)

add_library(__pycaffe SHARED ${python_srcs})
    caffe_default_properties(__pycaffe)
set_target_properties(__pycaffe PROPERTIES PREFIX "" OUTPUT_NAME "_caffe")
target_include_directories(__pycaffe PUBLIC ${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIR})
target_link_libraries(__pycaffe PUBLIC ${Caffe_LINK} ${PYTHON_LIBRARIES})




if(UNIX OR APPLE)
    set(__linkname "${PROJECT_SOURCE_DIR}/python/caffe/_caffe.so")
    add_custom_command(TARGET __pycaffe POST_BUILD
                       COMMAND ln -sf $<TARGET_LINKER_FILE:__pycaffe> "${__linkname}"
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/python/caffe/proto
                       COMMAND touch ${PROJECT_SOURCE_DIR}/python/caffe/proto/__init__.py
                       COMMAND cp ${proto_gen_folder}/*.py ${PROJECT_SOURCE_DIR}/python/caffe/proto/
                       COMMENT "Creating symlink ${__linkname} -> ${PROJECT_BINARY_DIR}/lib/_caffe${Caffe_POSTFIX}.so")
endif()

# ---[ Install
# scripts
file(GLOB python_files *.py requirements.txt)
install(FILES ${python_files} DESTINATION python)

# module
install(DIRECTORY caffe
    DESTINATION python
    FILES_MATCHING
    PATTERN "*.py"
    PATTERN "ilsvrc_2012_mean.npy"
    PATTERN "test" EXCLUDE
    )

# _caffe.so
install(TARGETS __pycaffe  DESTINATION python/caffe)




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

## ---[ Install
## scripts
#file(GLOB python_files *.py requirements.txt)
#install(FILES ${python_files} DESTINATION python)
#
## module
#install(DIRECTORY caffe
#        DESTINATION python
#        FILES_MATCHING
#        PATTERN "*.py"
#        PATTERN "ilsvrc_2012_mean.npy"
#        PATTERN "test" EXCLUDE
#        )

# _caffe.so
#install(TARGETS pyedgecaffe  DESTINATION python/caffe)

