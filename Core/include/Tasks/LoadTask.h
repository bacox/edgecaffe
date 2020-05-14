//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_LOADTASK_H
#define PIPELINE_LOADTASK_H

#include "Task.h"

namespace EdgeCaffe
{
    class LoadTask : public Task
    {
    public:
//        LoadTask(int id, int executionTime) : Task(id, executionTime)
//        {}
//
//        LoadTask()
//        {}

        LoadTask(
                int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0
                , int estimatedNeededMemory = 0
        );

        // Variables specific to this task
        std::string partialName;
        std::string pathToPartial;
        bool needsLoading = true;

        // Overridden functions of Task class
        std::string getTaskDescription() override;
        void run() override;
    };
}


#endif //PIPELINE_LOADTASK_H
