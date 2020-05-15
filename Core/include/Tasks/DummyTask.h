//
// Created by bacox on 18/04/2020.
//

#ifndef EDGECAFFE_DUMMYTASK_H
#define EDGECAFFE_DUMMYTASK_H

#include "Task.h"

namespace EdgeCaffe
{
    class DummyTask : public Task
    {
    public:
        bool isLoadingTask = false;
        void run() override;

        DummyTask(
                int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0
                , int estimatedNeededMemory = 0, bool isLoadingTask = false
        );

        std::string getTaskDescription() override;
    };
}


#endif //EDGECAFFE_DUMMYTASK_H
