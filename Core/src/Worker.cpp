//
// Created by bacox on 26/03/2020.
//

#include <iostream>
#include <thread>
#include "../include/Worker.h"

namespace EdgeCaffe
{


    void Worker::printMessage()
    {
        std::cout << "In thread loop of thread " << std::this_thread::get_id() << std::endl;
    }

    /**
     * Main function of the threaded worker.
     * This will run continuously during execution.
     * The worker checks if a task is available in the ready pool (`pool` variable)
     * If a task is available, the worker will:
     *  1. Retrieve the task
     *  2. Execute the task
     *  3. Move the task to the finished pool
     *  4. Set the execution property of the task to true
     */
    void Worker::Execution()
    {
        startTimeMeasuring();
        while (!action_stop && !AllowedToStop())
        {
            // Checking the pool to execute a new task
            Task *task = nullptr;
            if (scheduler->getNext(&task))
            {
                measureBusyTime();
                #ifdef MEMORY_CHECK_ON
                // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
                std::this_thread::sleep_for(std::chrono::milliseconds(1 ));
                #endif
                if(verbose)
                {
                    std::cout << std::this_thread::get_id() << " [" << workerId << "]" << " -> Running task ("
                              << task->networkId << "|t:"<< task->layerName << ") | " << task->id
                              << " = '" << task->getTaskDescription() << "'" << std::endl;
                }
                #ifdef MEMORY_CHECK_ON
                // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
                if(perf != nullptr){
                    perf->networkId = task->networkId;
                    perf->layerId = task->layerId;
                    perf->taskId = task->id;
                    if(task->layerId > -1)
                    {
                        perf->layerType = (*task->network_ptr)->layers()[task->layerId]->type();
                    } else
                    {
                        perf->layerType = "net-init";
                    }
                    perf->network = task->networkName;
                    perf->taskType = task->taskType;
                }
                #endif
                task->execute();
                // Move task to finished taskpool
                outpool->addTask(task);
                // Set task executed to true for the task dependency check
                task->executed = true;

#ifdef MEMORY_CHECK_ON
                // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
                std::this_thread::sleep_for(std::chrono::milliseconds(1 ));
                perf->networkId = -1;
                perf->network = "";
                #endif
            } else {
                // No task available --> wait
                measureIdleTime();
            }
            // Sleep for a short time to prevent the cpu from going insane
            // Maybe use <condition_variable> later on for better energy performance
            std::this_thread::sleep_for(std::chrono::nanoseconds (1));
        }
        endTimeMeasuring();
    }

    void Worker::run()
    {
        _thread = std::thread(&Worker::Execution, this);
    }

//    Worker::Worker(TaskPool *pool, TaskPool *outpool) : pool(pool), outpool(outpool)
//    {}
//
//    Worker::Worker(TaskPool *pool, TaskPool *outpool, int workerId) : pool(pool), outpool(outpool), workerId(workerId)
//    {}

    bool Worker::AllowedToStop()
    {
        if (scheduler.get() != nullptr)
        {
            if (scheduler->isEmpty())
                return allowed_to_stop;
            // Never stop if there are tasks left in the incomming pool
            return false;
        }
        return allowed_to_stop;
    }

    void Worker::measureIdleTime()
    {
        if(workerStatus == WorkerProfileLine::BUSY)
        {
            auto now = std::chrono::high_resolution_clock::now();
            workerProfile.back().stop = now;
            workerProfile.back().calcDuration();
            workerStatus = WorkerProfileLine::IDLE;
            workerProfile.push_back({workerStatus, now});
        }
    }

    void Worker::measureBusyTime()
    {
        if(workerStatus == WorkerProfileLine::IDLE)
        {
            auto now = std::chrono::high_resolution_clock::now();
            workerProfile.back().stop = now;
            workerProfile.back().calcDuration();
            workerStatus = WorkerProfileLine::BUSY;
            workerProfile.push_back({workerStatus, now});
        }
    }

    void Worker::startTimeMeasuring()
    {
        startTP = std::chrono::high_resolution_clock::now();
        workerProfile.push_back({workerStatus, startTP});
    }

    void Worker::endTimeMeasuring()
    {
        workerProfile.back().stop = std::chrono::high_resolution_clock::now();
        workerProfile.back().calcDuration();
        workerStatus = WorkerProfileLine::IDLE;
    }

    std::vector<std::string> Worker::workerProfileToCSVLines()
    {
        std::vector<std::string> lines;
        for( auto wp: workerProfile)
        {
            lines.push_back(wp.toCsvLine(startTP));
        }
        return lines;
    }

    Worker::Worker(
            const std::shared_ptr<TaskPool> &outpool, const std::shared_ptr<Scheduler> &scheduler, int workerId
    ) : outpool(outpool), scheduler(scheduler), workerId(workerId)
    {}
}