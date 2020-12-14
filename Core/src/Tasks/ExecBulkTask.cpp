//
// Created by bacox on 05/08/2020.
//

#include "../../include/Tasks/ExecBulkTask.h"

void EdgeCaffe::ExecBulkTask::run()
{
    (*network_ptr)->Forward();
}

std::string EdgeCaffe::ExecBulkTask::getTaskDescription()
{
    return "Exec Bulk Task";
}

EdgeCaffe::ExecBulkTask::ExecBulkTask(
        int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
) : ExecTask(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory)
{}
