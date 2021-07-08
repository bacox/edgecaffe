//
// Created by bacox on 27/07/2020.
//


#include <TaskPool/FCFSTaskPool.h>
#include <Scheduler/FCFSScheduler.h>
#include "../../include/Orchestrator/BulkOrchestrator.h"

void EdgeCaffe::BulkOrchestrator::setup()
{
//    taskPools.emplace_back(std::make_shared<FCFSTaskPool>(mc, nr));
    schedulers.emplace_back(std::make_shared<FCFSScheduler>(mc, nr));
    workers.emplace_back(std::make_shared<Worker>(outPool, schedulers.front(), 0));
    enforceInterDependencies = false;
#ifdef MEMORY_CHECK_ON
    // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
    workers.back()->perf = &perf;
#endif
}

void EdgeCaffe::BulkOrchestrator::checkBagOfTasks()
{
    // Check if new tasks are available to insert in the taskpool
    for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
    {
        // remove odd numbers
        std::shared_ptr<Task> task = *it;
        if (!task->waitsForOtherTasks())
        {
            bagOfTasks.erase(it--);
//            if (task->hasPoolAssigned())
//            {
//                int poolId = task->getAssignedPoolId();
//                taskPools[poolId]->addTask(task);
//            } else
//            {
            schedulers.front()->addTask(task);
//                taskPools.front()->addTask(task);
//            }
            task->measureTime(Task::TIME::TO_READY);
        }
    }
}
