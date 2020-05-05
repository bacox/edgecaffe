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
            LayerProfile lp;
            lp.layerId = idx++;
            lp.layerName = layer;
            networkProfile.push_back(lp);
        }
    }

    void InferenceOutput::setLoadingTime(Task *task)
    {
        int layerId = task->layerId;
        networkProfile[layerId].loading = task->profileLine;
    }

    void InferenceOutput::setExecutionTime(Task *task)
    {
        networkProfile[task->layerId].executing = task->profileLine;
    }

    std::vector<std::string> InferenceOutput::toCsvLines()
    {
        std::vector<std::string> lines;
        std::string sep = ",";
        for (const LayerProfile &pl : networkProfile)
        {

            std::string line = networkName + sep + std::to_string(pl.layerId) + sep + pl.layerName + sep +
                               std::to_string(pl.loading.duration) + sep +
                               std::to_string(pl.executing.duration) + sep + policy;
            lines.push_back(line);
        }
        return lines;
    }
}