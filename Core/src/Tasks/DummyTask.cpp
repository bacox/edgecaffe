//
// Created by bacox on 18/04/2020.
//

#include <thread>
#include "Tasks/DummyTask.h"
namespace EdgeCaffe {

    void DummyTask::run()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(estimatedExecutionTime));
    }

    std::string DummyTask::getTaskDescription()
    {
        if(isLoadingTask)
            return "Dummy Loading Task " + std::to_string(id);
        else
            return "Dummy Execution Task " + std::to_string(id);
    }

    DummyTask::DummyTask(
            int id, int networkId, const std::string &taskName, int estimatedExecutionTime, int estimatedNeededMemory
            , bool isLoadingTask
    ) : Task(id, networkId, taskName, estimatedExecutionTime, estimatedNeededMemory), isLoadingTask(isLoadingTask)
    {}
}