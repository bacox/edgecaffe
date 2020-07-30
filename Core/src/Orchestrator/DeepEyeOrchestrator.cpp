//
// Created by bacox on 28/07/2020.
//

#include <Scheduler/FCFSScheduler.h>
#include "../../include/Orchestrator/DeepEyeOrchestrator.h"

void EdgeCaffe::DeepEyeOrchestrator::setup()
{
    // Create a scheduler for the fc layers
    schedulers.emplace_back(std::make_shared<FCFSScheduler>(mc, nr));
    // Create a scheduler for the conv layers
    schedulers.emplace_back(std::make_shared<FCFSScheduler>(mc, nr));
    workers.emplace_back(std::make_shared<Worker>(outPool, schedulers[0], 0));
    workers.emplace_back(std::make_shared<Worker>(outPool, schedulers[1], 1));
    enforceInterDependencies = false;
}

void EdgeCaffe::DeepEyeOrchestrator::checkBagOfTasks()
{
    // Check if new tasks are available to insert in the taskpool
    for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
    {
        // remove odd numbers
        Task *task = *it;
        if (!task->waitsForOtherTasks())
        {
            bagOfTasks.erase(it--);
            int poolId = task->getAssignedPoolId();
            schedulers[poolId]->addTask(task);
            task->measureTime(Task::TIME::TO_READY);
        }
    }
}
