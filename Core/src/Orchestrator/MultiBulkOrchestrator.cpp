//
// Created by bacox on 13/08/2020.
//

#include <Scheduler/FCFSScheduler.h>
#include "../../include/Orchestrator/MultiBulkOrchestrator.h"

void EdgeCaffe::MultiBulkOrchestrator::setup()
{
//    taskPools.emplace_back(std::make_shared<FCFSTaskPool>(mc, nr));
    schedulers.emplace_back(std::make_shared<FCFSScheduler>(mc, nr));
    for (int i = 0; i < numberOfWorkers; ++i)
    {
        workers.emplace_back(std::make_shared<Worker>(outPool, schedulers.front(), i));
    }
    enforceInterDependencies = false;
#ifdef MEMORY_CHECK_ON
    // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
    workers.back()->perf = &perf;
#endif
}

void EdgeCaffe::MultiBulkOrchestrator::checkBagOfTasks()
{
    // Check if new tasks are available to insert in the taskpool
    for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
    {
        // remove odd numbers
        std::shared_ptr<Task> task = *it;
        if (!task->waitsForOtherTasks())
        {
            bagOfTasks.erase(it--);
            schedulers.front()->addTask(task);
            task->measureTime(Task::TIME::TO_READY);
        }
    }
}