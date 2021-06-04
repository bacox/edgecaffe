//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_INFERENCETASK_H
#define EDGECAFFE_INFERENCETASK_H


#include "InferenceNetwork.h"
#include "InferenceOutput.h"

namespace EdgeCaffe
{
    /**
     * Stucture to hold the references and the data of the network, tasks and the layers during execution
     */
    struct InferenceTask
    {
        std::string pathToNetwork;
        std::vector<std::string> pathToData;
        InferenceNetwork *net;

        bool finished = false;

        InferenceOutput output;

        void dealloc()
        {
            net->networkProfile.measure(NetworkProfile::STOP);
            output.netProfile = net->networkProfile;
            auto ptr = net->subTasks.front()->net_ptr;
            std::vector <std::string> layerNames;
            if (ptr)
            {
                layerNames = net->subTasks.front()->net_ptr->layer_names();
            } else
            {
                for (auto t : net->tasks)
                {
                    std::string layerName = "layer-" + std::to_string(t->layerId);
                    layerNames.push_back(layerName);
                }
                auto last = std::unique(layerNames.begin(), layerNames.end());
                layerNames.erase(last, layerNames.end());
            }
            output.initFromLayerVector(layerNames);
//
            for (auto task : net->tasks)
            {
                task->measureTime(Task::TIME::FINISHED);
                if (dynamic_cast<LoadTask *>(task))
                {
//                // Load Task
                    output.setLoadingTime(task);
                    output.addTaskProfile(task, true);
                }
                if (dynamic_cast<ExecTask *>(task))
                {
//                // Load Task
                    output.setExecutionTime(task);
                    output.addTaskProfile(task, false);
                }
                if (auto dt = dynamic_cast<DummyTask *>(task))
                {
                    if (dt->isLoadingTask)
                    {
                        output.setLoadingTime(task);
                        output.addTaskProfile(task, true);
                    } else
                    {
                        output.setExecutionTime(task);
                        output.addTaskProfile(task, false);
                    }
                }
            }
            MemoryCounter *mc = net->mc;
            double networkMemoryUsage = net->maxMemoryUsage;
            delete net;

            mc->releaseMemory(networkMemoryUsage);

        };
    };
}
#endif //EDGECAFFE_INFERENCETASK_H
