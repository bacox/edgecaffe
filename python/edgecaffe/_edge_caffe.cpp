#include <Python.h>  // NOLINT(build/include_alpha)
//
//// Produce deprecation warnings (needs to come before arrayobject.h inclusion).
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//
//#include <boost/make_shared.hpp>
#include <boost/python.hpp>
//#include <boost/python/raw_function.hpp>
//#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <numpy/arrayobject.h>
//
//// these need to be included after boost on OS X
#include <string>  // NOLINT(build/include_order)
#include <vector>  // NOLINT(build/include_order)
#include <fstream>  // NOLINT
#include <iostream>
#include <BaseNet.h>

#define BOOST_LIB_NAME "boost_numpy35"
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <numpy/arrayobject.h>
//#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

//// Temporary solution for numpy < 1.7 versions: old macro, no promises.
//// You're strongly advised to upgrade to >= 1.7.
#ifndef NPY_ARRAY_C_CONTIGUOUS
#define NPY_ARRAY_C_CONTIGUOUS NPY_C_CONTIGUOUS
#define PyArray_SetBaseObject(arr, x) (PyArray_BASE(arr) = (x))
#endif
//
///* Fix to avoid registration warnings in pycaffe (#3960) */
#define BP_REGISTER_SHARED_PTR_TO_PYTHON(PTR) do { \
  const boost::python::type_info info = \
    boost::python::type_id<shared_ptr<PTR > >(); \
  const boost::python::converter::registration* reg = \
    boost::python::converter::registry::query(info); \
  if (reg == NULL) { \
    bp::register_ptr_to_python<shared_ptr<PTR > >(); \
  } else if ((*reg).m_to_python == NULL) { \
    bp::register_ptr_to_python<shared_ptr<PTR > >(); \
  } \
} while (0)
//
namespace bp = boost::python;
//
namespace {
    namespace np = boost::python::numpy;

    using boost::shared_ptr;

    void BaseNet_SetInputFromString(EdgeCaffe::BaseNet& net, const std::string& pathToImage, bool use_scales) {
        cv::Mat img = cv::imread(pathToImage);
        net.setInput(img, use_scales);
    }

    int BaseNet_NumberOfLayers(EdgeCaffe::BaseNet& net){
        return net.num_layers;
    }



BOOST_PYTHON_MODULE(_edge_caffe) {

        bp::class_<EdgeCaffe::BaseNet, shared_ptr<EdgeCaffe::BaseNet >, boost::noncopyable >("BaseNet",
                bp::init<std::string>())
                //constructor
                .def("init", &EdgeCaffe::BaseNet::init)
//                .def("setInput", &BaseNet_SetInputFromString)
                .def("setInputFromString", &BaseNet_SetInputFromString)
                .def("loadNetworkStructure", &EdgeCaffe::BaseNet::loadNetworkStructure)
                .def("loadInputToNetwork", &EdgeCaffe::BaseNet::loadInputToNetwork)
                .def("runNetwork", &EdgeCaffe::BaseNet::runNetwork)
                .def("showResult", &EdgeCaffe::BaseNet::showResult)
                .def("numLayers", &BaseNet_NumberOfLayers)

                .def("loadPartialLayer", &EdgeCaffe::BaseNet::loadPartialLayer)
                .def("runPartialLayer", &EdgeCaffe::BaseNet::runPartialLayer)
                .def("unloadLayer", &EdgeCaffe::BaseNet::unloadLayer)
                ;
        BP_REGISTER_SHARED_PTR_TO_PYTHON(EdgeCaffe::BaseNet);


  // boost python expects a void (missing) return value, while import_array
  // returns NULL for python3. import_array1() forces a void return value.
  import_array1();
}

}  // namespace



