//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_INFERENCENETWORK_H
#define PIPELINE_INFERENCENETWORK_H

#include <vector>
//#include <net.hpp>
#include "Tasks/Task.h"
#include "InferenceSubTask.h"
#include "TaskPool.h"
#include <string>
#include <opencv2/core/mat.hpp>
//#include "../thirdparty/caffe/include/caffe/caffe.hpp"
#include <memory>
#include <yaml-cpp/yaml.h>

namespace EdgeCaffe
{
    struct LayerDescription
    {
        int layerId = 0;
        std::string type = "";
        std::string name = "";
        std::string partialFileName = "";
        long estimated_loading = 0;
        long estimated_execution = 0;
        bool isConv = true;
        bool hasModelFile = true;

        friend std::ostream &operator<<(std::ostream &os, const LayerDescription &description)
        {
            os << "layerId: " << description.layerId << " type: " << description.type << " name: " << description.name
               << " partialFileName: " << description.partialFileName << " estimated_loading: "
               << description.estimated_loading << " estimated_execution: " << description.estimated_execution
               << " isConv: " << description.isConv << " hasModelFile: " << description.hasModelFile;
            return os;
        }

        YAML::Node toYaml()
        {
            YAML::Node node;
            node["id"] = layerId;
            node["name"] = name;
            node["partialFile"] = partialFileName;
            node["estimated_time_loading"] = estimated_loading;
            node["estimated_time_execution"] = estimated_execution;
            node["isConvLayer"] = isConv;
            node["hasModelFile"] = hasModelFile;
            return node;
        }

        static LayerDescription FromYaml(YAML::Node node, std::string pathPrefix = ".")
        {
            LayerDescription rhs;
            rhs.layerId = node["id"].as<int>();
            rhs.name = node["name"].as<std::string>();
            rhs.partialFileName = pathPrefix + "/" + node["partialFile"].as<std::string>();
            rhs.estimated_loading = node["estimated_time_loading"].as<long>();
            rhs.estimated_execution = node["estimated_time_execution"].as<long>();
            rhs.isConv = node["isConvLayer"].as<bool>();
            rhs.hasModelFile = node["hasModelFile"].as<bool>();
            return rhs;
        }
    };

    class InferenceNetwork
    {
    private:
        std::vector<LayerDescription> layerDescriptions;
    public:
        std::vector<Task *> tasks;
        bool use_scales = false;
        std::string dataPath;
        const std::vector<Task *> &getTasks() const;
        NetworkProfile networkProfile;
        std::vector<TaskPool*> taskpools;
        std::vector<Task *> *bagOfTasks_ptr;

        std::vector<InferenceSubTask *> subTasks;
    protected:

        std::string pathToDescription;

        void preprocess(bool use_scales = false);

    public:


        static int TASKID_COUNTER;
        static int NETWORKID_COUNTER;

        virtual ~InferenceNetwork();

        /**
     *  @Brief: The data transform from OpenCV to caffe Blob
     *
     *  @param image: OpenCV Mat data vector
     *  @Warning: Template function must be defined in the .hpp file to avoid
     *            linking error
     */
        template<typename DType>

        void OpenCV2Blob(
                const std::vector<cv::Mat> &channels, caffe::Net<DType> &net
        )
//                     std::shared_ptr<caffe::Net<DType>> &net)
        {
            caffe::Blob<DType> *input_layer = net.input_blobs()[0];
            DType *input_data = input_layer->mutable_cpu_data();

            for (const cv::Mat &ch: channels)
            {
                for (auto i = 0; i != ch.rows; ++i)
                {
                    std::memcpy(input_data, ch.ptr<DType>(i), sizeof(DType) * ch.cols);
                    input_data += ch.cols;
                }
            }
        }


        InferenceNetwork(const std::string &pathToDescription);

        virtual void init(YAML::Node &description);
        virtual void init();

        virtual void setInput(cv::Mat &input, bool use_scales = false);

        virtual void loadInputToNetwork();

        virtual void loadNetworkStructure();

        virtual void createTasks();

        void createTasksConvFC();

        void createTasksBulk();

        void createTasksLinear();

        virtual void createTasks(int splittingPolicy);

        void showResult();

        bool isFinished();

        void createTasksBase();

    };
}

#endif //PIPELINE_INFERENCENETWORK_H
