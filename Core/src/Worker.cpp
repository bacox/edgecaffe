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
     * This will run continously during execution.
     * The worker checks if a task is available in the ready pool (`pool` variable)
     * If a task is available, the worker will:
     *  1. Retrieve the task
     *  2. Execute the task
     *  3. Move the task to the finished pool
     *  4. Set the execution property of the task to true
     */
    void Worker::Execution()
    {
        while (!action_stop && !AllowedToStop())
        {
            // Checking the pool to execute a new task
            Task *task = nullptr;
            if (pool->getNext(&task))
            {
                std::cout << std::this_thread::get_id() << " [" << workerId << "]" << " -> Running task (" << task->networkId << ") | "<< task->id
                          << " = '" << task->getTaskDescription() << "'" << std::endl;
                task->execute();
                // Move task to finished taskpool
                outpool->addTask(task);
                // Set task executed to true for the task dependency check
                task->executed = true;
            }
            // Sleep for a short time to prevent the cpu from going insane
            // Maybe use <condition_variable> later on for better energy performance
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Worker::run()
    {
        _thread = std::thread(&Worker::Execution, this);
    }

    Worker::Worker(TaskPool *pool, TaskPool *outpool) : pool(pool), outpool(outpool)
    {}

    Worker::Worker(TaskPool *pool, TaskPool *outpool, int workerId) : pool(pool), outpool(outpool), workerId(workerId)
    {}

    bool Worker::AllowedToStop()
    {
        if (pool != nullptr)
        {
            if (pool->isEmpty())
                return allowed_to_stop;
            // Never stop if there are tasks left in the incomming pool
            return false;
        }
        return allowed_to_stop;
    }
}