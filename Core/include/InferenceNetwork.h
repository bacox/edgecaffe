//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_INFERENCENETWORK_H
#define PIPELINE_INFERENCENETWORK_H

#include <vector>
//#include <net.hpp>
#include "Tasks/Task.h"
#include "InferenceSubTask.h"
#include <string>
#include <opencv2/core/mat.hpp>
//#include "../thirdparty/caffe/include/caffe/caffe.hpp"
#include <memory>

namespace EdgeCaffe {
    class InferenceNetwork {
    private:
    public:
        std::vector<Task *> tasks;

        const std::vector<Task *> &getTasks() const;

        std::vector<InferenceSubTask *> subTasks;
    private:

        std::string pathToDescription;

        void preprocess(bool use_scales = false);

    public:


        static int TASKID_COUNTER;

        virtual ~InferenceNetwork();

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


        InferenceNetwork(const std::string &pathToDescription);

        void init();

        void setInput(cv::Mat &input, bool use_scales = false);

        void loadInputToNetwork();

        void loadNetworkStructure();


        void createTasks();

        void createTasksConvFC();

        void createTasksBulk();

        void createTasksLinear();

        void createTasks(int splittingPolicy);

        void showResult();

        bool isFinished();

    };
}

#endif //PIPELINE_INFERENCENETWORK_H
