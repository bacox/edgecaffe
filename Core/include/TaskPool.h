//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_TASKPOOL_H
#define PIPELINE_TASKPOOL_H


#include <list>
#include <mutex>
#include "Tasks/Task.h"

class TaskPool {
private:
    std::mutex mtx;

public:

    std::list<Task*> pool;

    void addTask(Task* t_ptr);

    bool isEmpty();

    Task* getNext();

    bool getNext(Task **task);
};


#endif //PIPELINE_TASKPOOL_H
