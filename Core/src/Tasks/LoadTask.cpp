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
//            std::cout << "Attempting to load the file " << pathToPartial << std::endl;
            (*network_ptr)->CopyTrainedLayersFrom(pathToPartial); // layer 12
//            std::cout << "Done loading file " << pathToPartial << std::endl;
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
