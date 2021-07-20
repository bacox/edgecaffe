//
// Created by bacox on 12/04/2020.
//

#ifndef EDGECAFFE_INFERENCEOUTPUT_H
#define EDGECAFFE_INFERENCEOUTPUT_H


#include <Profiler/NetworkProfiler.h>
#include <opencv2/core/types.hpp>
#include <map>
#include <memory>
namespace EdgeCaffe
{
    class Task;
    struct OutputData
    {
        int rows = 0;
        int columns = 0;

        int numItems()
        {
            return rows * columns;
        }


    };

    class InferenceOutput
    {
        cv::Size inputDim = cv::Size(0, 0);
        size_t inputSize = 0;
        std::vector<LayerProfile> networkProfile;
        std::vector<TaskProfile> taskProfile;
    public:

        std::map<std::string, double> networkData;

        NetworkProfile netProfile;

        enum EVENT
        {
            WAITING_UP,
            WAITING_DOWN,
            RUNNING_UP,
            RUNNING_DOWN,
            FINISHED_UP,
            FINISHED_DOWN
        };
        struct event {
            long time;
            EVENT type;
        };
        void setLoadingTime(std::shared_ptr<Task> task);

        void setExecutionTime(std::shared_ptr<Task> task);

        void addTaskProfile(std::shared_ptr<Task> task, bool isLoading = false);

        std::vector<std::string> toCsvLines();

        std::string networkName;

        void initFromLayerVector(const std::vector<std::string>& layerNames);

        std::string policy;

        void getTaskEvents(std::vector<event> &list, std::chrono::time_point<std::chrono::system_clock> ref);

        std::vector<std::string> static calculateTaskProfile(std::vector<event> events);
    };

}
#endif //EDGECAFFE_INFERENCEOUTPUT_H
