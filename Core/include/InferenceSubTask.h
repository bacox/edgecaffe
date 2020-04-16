//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_INFERENCESUBTASK_H
#define PIPELINE_INFERENCESUBTASK_H


//#include <net.hpp>
#include <string>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include "Tasks/Task.h"

namespace EdgeCaffe
{
    class InferenceSubTask
    {
    public:
        caffe::Net<float> *net_ptr;
        std::string pathToDescription;
        std::string pathToModelFile;
        std::string modelFileName;
        std::string pathToParamFile;
        std::string pathToPartials;
        cv::Size inputSize;
        cv::Scalar modelMeanValues;
        std::string networkName;
        std::string basePath;
        bool hasInputLayer; // maybe set to explicit value?
        int num_layers;
        int num_conv;
        int num_fc;
        std::vector<std::string> partialNames;
        std::vector<std::string> resultVector;
        cv::Mat inputData;
        cv::Mat origInputData;
        std::vector<Task *> tasks;

        virtual ~InferenceSubTask();

        Task *lastTask;
        Task *firstTask = nullptr;

    };
}

#endif //PIPELINE_INFERENCESUBTASK_H
