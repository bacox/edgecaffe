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
    const int bufferSize = 10;
    std::vector<Task *> buffer;
    buffer.reserve(bufferSize);
    int initCount = 0;
    // Check if new tasks are available to insert in the taskpool
    for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
    {
        // remove odd numbers
        Task *task = *it;
        if (!task->waitsForOtherTasks())
        {
            bagOfTasks.erase(it--);
            if(task->t_type == Task::INIT && initCount < bufferSize)
            {
                buffer.push_back(task);
                initCount++;
            } else{
                schedulers.front()->addTask(task);
                task->measureTime(Task::TIME::TO_READY);
            }
        }
    }
    if(!initCount)
        return;
    // Make sure the init tasks are inserted in correct order to ensure SNF
    std::sort(buffer.begin(), buffer.end(), Task::compByEstTime);
    for(Task *task : buffer){
        schedulers.front()->addTask(task);
        task->measureTime(Task::TIME::TO_READY);
    }
}

void EdgeCaffe::MasaOrchestrator::submitInferenceTask(const EdgeCaffe::Arrival arrivalTask, bool use_scales)
{
    Orchestrator::submitInferenceTask(arrivalTask, use_scales);
//    std::sort(bagOfTasks.begin(), bagOfTasks.end(), Task::compByEstTime);
}
