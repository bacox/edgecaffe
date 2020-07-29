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
#include "MemoryCounter.h"
#include "Types.h"
#include <string>
#include <opencv2/core/mat.hpp>
//#include "../thirdparty/caffe/include/caffe/caffe.hpp"
#include <memory>
#include <yaml-cpp/yaml.h>
#include <TaskPool/AbstractTaskPool.h>

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
        double maxMemoryUsage = 0;
        double meanExecutionTime = std::numeric_limits<double>::max();
        std::vector<Task *> tasks;
        bool use_scales = false;
        std::string dataPath;
        const std::vector<Task *> &getTasks() const;
        NetworkProfile networkProfile;
//        std::vector<TaskPool*> taskpools;
        std::vector<std::shared_ptr<AbstractTaskPool>> taskpools;
        std::vector<Task *> *bagOfTasks_ptr;
        int networkId;

        std::vector<InferenceSubTask *> subTasks;
    protected:

        std::string pathToDescription;
        bool * dependencyCondition;

        void preprocess(bool use_scales = false);

    public:

        MemoryCounter *mc;


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


        InferenceNetwork(const std::string &pathToDescription, bool *dependencyCondition);

        virtual void init(YAML::Node &description);
        virtual void init();

        virtual void setInput(cv::Mat &input, bool use_scales = false);

        virtual void loadInputToNetwork();

        virtual void loadNetworkStructure();

        virtual void createPartialTasks();

        void createTasksConvFC();

        void createTasksBulk();

        void createTasksLinear();
        void createTasksExecPrio();

//        virtual void createTasks(int splittingPolicy);
        virtual void createTasks(Type::MODE_TYPE mode);

        void showResult();

        bool isFinished();

        std::string tasksToDotDebug();

        Task * createInitTask(InferenceSubTask *dnn);
        Task * createLoadTask(InferenceSubTask *dnn, const LayerDescription &descr);
        Task * createExecTask(InferenceSubTask *dnn, const LayerDescription &descr);
    };
}

#endif //PIPELINE_INFERENCENETWORK_H
