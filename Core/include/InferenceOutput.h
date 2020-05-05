//
// Created by bacox on 12/04/2020.
//

#ifndef EDGECAFFE_INFERENCEOUTPUT_H
#define EDGECAFFE_INFERENCEOUTPUT_H


#include <Profiler/NetworkProfiler.h>
#include <opencv2/core/types.hpp>
#include <Tasks/Task.h>

namespace EdgeCaffe
{
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

    public:
        void setLoadingTime(Task *task);

        void setExecutionTime(Task *task);

        std::vector<std::string> toCsvLines();

        std::string networkName;

        void initFromLayerVector(const std::vector<std::string> layerNames);

        std::string policy;
    };

}
#endif //EDGECAFFE_INFERENCEOUTPUT_H
