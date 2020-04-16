//
// Created by bacox on 12/04/2020.
//

#include "InferenceOutput.h"

namespace EdgeCaffe
{

    void InferenceOutput::initFromLayerVector(const std::vector<std::string> layerNames)
    {
        int idx = 0;
        for (const auto &layer : layerNames)
        {
            _LayerProfile lp;
            lp.layerId = idx++;
            lp.layerName = layer;
            networkProfile.push_back(lp);
        }
    }

    void InferenceOutput::setLoadingTime(Task *task)
    {
//    task->layerId;
//    task->profileLine;
//    networkProfile[task->layerId];
//    networkProfile[task->layerId].loadingProfile;
//    ProfileLine line = task->profileLine;
//    networkProfile[task->layerId].loadingProfile = task->profileLine;
        int layerId = task->layerId;
        networkProfile[layerId].loadingProfile = task->profileLine;
    }

    void InferenceOutput::setExecutionTime(Task *task)
    {
        networkProfile[task->layerId].ExecutionProfile = task->profileLine;
    }

    std::vector<std::string> InferenceOutput::toCsvLines()
    {
        std::vector<std::string> lines;
        std::string sep = ",";
        for (auto pl : networkProfile)
        {

            std::string line = networkName + sep + std::to_string(pl.layerId) + sep + pl.layerName + sep +
                               std::to_string(pl.loadingProfile.duration) + sep +
                               std::to_string(pl.ExecutionProfile.duration) + sep + policy;
            lines.push_back(line);
        }
        return lines;
    }
}