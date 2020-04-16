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

    void Worker::Execution()
    {
        while (!action_stop && !AllowedToStop())
        {
            // Checking the pool to execute a new task
            Task *task = nullptr;
            if (pool->getNext(&task))
            {
                std::cout << std::this_thread::get_id() << " [" << workerId << "]" << " -> Running task " << task->id
                          << " = '" << task->getTaskDescription() << "'" << std::endl;
                task->execute();
                outpool->addTask(task);
                task->executed = true;
            }

//        printMessage();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void Worker::run()
    {
        _thread = std::thread(&Worker::Execution, this);
    }

    Worker::Worker(TaskPool *pool, TaskPool *outpool) : pool(pool), outpool(outpool)
    {

    }

    Worker::Worker(TaskPool *pool, TaskPool *outpool, int workerId) : pool(pool), outpool(outpool), workerId(workerId)
    {

    }

    bool Worker::AllowedToStop()
    {
        if (pool != nullptr)
        {
            if (pool->isEmpty())
                return allowed_to_stop;
            return false;
        }
        return allowed_to_stop;


    }

    void Worker::registerTaskPool(TaskPool *pool)
    {
        this->pool = pool;
    }
}