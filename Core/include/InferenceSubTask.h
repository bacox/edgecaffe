//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_INFERENCESUBTASK_H
#define PIPELINE_INFERENCESUBTASK_H


//#include <net.hpp>
#include <string>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <caffe/net.hpp>


namespace EdgeCaffe
{
    class Task;
    class InferenceSubTask
    {
    public:
        caffe::Net<float> *net_ptr = nullptr;
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
        std::vector<std::shared_ptr<Task>> tasks;

        virtual ~InferenceSubTask();

        std::shared_ptr<Task> lastTask = nullptr;
        std::shared_ptr<Task> firstTask = nullptr;

        // Task linking values
        std::shared_ptr<Task> conv_load_first = nullptr;
        std::shared_ptr<Task> conv_load_last = nullptr;
        std::shared_ptr<Task> conv_exec_first = nullptr;
        std::shared_ptr<Task> conv_exec_last = nullptr;
        std::shared_ptr<Task> fc_load_first = nullptr;
        std::shared_ptr<Task> fc_load_last = nullptr;
        std::shared_ptr<Task> fc_exec_first = nullptr;
        std::shared_ptr<Task> fc_exec_last = nullptr;
    };
}

#endif //PIPELINE_INFERENCESUBTASK_H
