//
// Created by bacox on 31/03/2020.
//

#ifndef PIPELINE_BASENET_H
#define PIPELINE_BASENET_H

#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>
#include <caffe/net.hpp>
//#include <net.hpp>
#include "Profiler/NetworkProfiler.h"

namespace EdgeCaffe {
    class BaseNet {
    public:
        std::string pathToDescription;
//    boost::shared_ptr<caffe::Net<float>> net;
//    obj(new Obj);
//    std::shared_ptr<caffe::Net<float>> net;
        caffe::Net<float> *net;
        std::string pathToModelFile;
        std::string modelFileName;
        std::string pathToParamFile;
        std::string pathToPartials;
        cv::Size inputSize;
        cv::Scalar modelMeanValues;
        std::string networkName;
        std::string basePath;
        bool hasInputLayer;
        int num_layers;
        int num_conv;
        int num_fc;
        std::vector<std::string> partialNames;
        cv::Mat inputData;
        std::vector<std::string> resultVector;

        NetworkProfiler profiler = NetworkProfiler("");

        /**
     *  @Brief: The data transform from OpenCV to caffe Blob
     *
     *  @param image: OpenCV Mat data vector
     *  @Warning: Template function must be defined in the .hpp file to avoid
     *            linking error
     */
        template<typename DType>

        void OpenCV2Blob(const std::vector<cv::Mat> &channels,
                         caffe::Net<DType> &net)
//                     std::shared_ptr<caffe::Net<DType>> &net)
        {
            caffe::Blob<DType> *input_layer = net.input_blobs()[0];
            DType *input_data = input_layer->mutable_cpu_data();

            for (const cv::Mat &ch: channels) {
                for (auto i = 0; i != ch.rows; ++i) {
                    std::memcpy(input_data, ch.ptr<DType>(i), sizeof(DType) * ch.cols);
                    input_data += ch.cols;
                }
            }
        }


        BaseNet(std::string pathToDescription);

        void init();

        void setInput(cv::Mat &input, bool use_scales = false);

        void loadInputToNetwork();

        void preprocess(bool use_scales = false);


        void loadNetworkStructure();

        void loadPartialLayer(int i);

        void runPartialLayer(int i);

        void bulkLoadLayers();

        void bulkRunLayers();

        void runNetwork(bool bulk);

        void unloadLayer(int i);

        void createNewProfiler();

        void showResult();

        virtual ~BaseNet();

        NetworkProfiler getProfiler();
    };
}


#endif //PIPELINE_BASENET_H
