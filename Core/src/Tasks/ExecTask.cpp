//
// Created by bacox on 30/03/2020.
//

#include <thread>
#include <chrono>
#include "../../include/Tasks/ExecTask.h"

namespace EdgeCaffe
{

    void ExecTask::run()
    {
        // Execute layer
        (*network_ptr)->layers()[layerId]->Forward((*network_ptr)->bottom_vecs()[layerId], (*network_ptr)->top_vecs()[layerId]);
        // Unload layer
        (*network_ptr)->layers_unsafe()[layerId].reset();
    }

    std::string ExecTask::getTaskDescription()
    {
        return "Exec Task layer " + std::to_string(layerId);
    }

    ExecTask::ExecTask(
            int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
    ) : Task(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory)
    {}
}