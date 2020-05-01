//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_TASKPOOL_H
#define PIPELINE_TASKPOOL_H


#include <list>
#include <mutex>
#include "Tasks/Task.h"

namespace EdgeCaffe
{
    class TaskPool
    {
        /**
         * The task pool holds references to a set of tasks.
         * Some pools are available at multiple core and/or threads.
         * No actual data is held or managed in taskpools, it only holds pointers to tasks.
         */
    private:
        std::mutex mtx;

    public:

        int poolId = -1;
        // Use a list of pointers to prevent copying the memory
        std::list<Task *> pool;

        /**
         * Add a reference of a task to the taskpool
         * @param t_ptr     Task pointer
         */
        void addTask(Task *t_ptr);

        /**
         * Checks if the taskpool is empty
         * @return Boolean      True if empty, false if not empty
         */
        bool isEmpty();

        /**
         * Gets the next task from the task pool and binds it to the provided pointer in the argument
         * @param task      Double pointer to store the reference to the task in.
         * @return Boolan   Returns false if the pool is empty and true if a task was bound to the given pointer
         */
        bool getNext(Task **task);
    };
}


#endif //PIPELINE_TASKPOOL_H
