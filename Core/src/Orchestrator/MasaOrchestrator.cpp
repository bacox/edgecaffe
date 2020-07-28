//
// Created by bacox on 27/07/2020.
//

#include <TaskPool/TypePriorityTaskPool.h>
#include <Scheduler/MasaScheduler.h>
#include "../../include/Orchestrator/MasaOrchestrator.h"

void EdgeCaffe::MasaOrchestrator::setup()
{
    schedulers.emplace_back(std::make_shared<MasaScheduler>(mc, nr));
    for (int i = 0; i < numberOfWorkers; ++i)
    {
        workers.emplace_back(std::make_shared<Worker>(outPool, schedulers.front(), i));
    }
}

void EdgeCaffe::MasaOrchestrator::checkBagOfTasks()
{
    // Check if new tasks are available to insert in the taskpool
    for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
    {
        // remove odd numbers
        Task *task = *it;
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
