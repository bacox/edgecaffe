//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_WORKER_H
#define PIPELINE_WORKER_H


#include "TaskPool.h"
#include <thread>

class Worker {
public:
    bool action_stop = false;
    bool allowed_to_stop = false;
    std::thread _thread;
    TaskPool* pool;
    TaskPool* outpool;

    int workerId = -1;

    void printMessage();
    void Execution();
    void run();

    bool AllowedToStop();

    Worker(TaskPool *pool, TaskPool *outpool);
    Worker(TaskPool *pool, TaskPool *outpool, int workerId);

    void registerTaskPool(TaskPool *pool);
};


#endif //PIPELINE_WORKER_H
