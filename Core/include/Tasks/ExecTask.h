//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_EXECTASK_H
#define PIPELINE_EXECTASK_H

#include "Task.h"

namespace EdgeCaffe
{
    class ExecTask : public Task
    {
    public:
        void run() override;

        ExecTask(
                int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0
                , int estimatedNeededMemory = 0
        );

        std::string getTaskDescription() override;
    };
}

#endif //PIPELINE_EXECTASK_H
