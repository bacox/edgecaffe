//
// Created by bacox on 30/03/2020.
//

#include <thread>
#include <chrono>
#include "../../include/Tasks/LoadTask.h"

namespace EdgeCaffe
{

    void LoadTask::run()
    {
        if (needsLoading)
        {
            (*network_ptr)->CopyTrainedLayersFrom(pathToPartial); // layer 12
        } else
        {
            std::cout << "Running the loading task (Dummy not actual loading)" << std::endl;
        }
    }

    std::string LoadTask::getTaskDescription()
    {
        return "Load Task layer " + std::to_string(layerId);
    }

    LoadTask::LoadTask(
            int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
    ) : Task(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory)
    {

    }
}
