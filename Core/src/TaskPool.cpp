//
// Created by bacox on 26/03/2020.
//

#include "../include/TaskPool.h"
namespace EdgeCaffe
{
    /**
     * Add a reference of a task to the taskpool
     * @param t_ptr     Task pointer
     */
    void TaskPool::addTask(Task *t_ptr)
    {
        // Use lock-guard for the mutex in the same way as a smart pointer
        // The mutex will be released when the lock-guard goes out of scope (end of function)
        std::lock_guard guard(mtx);
        switch (policy)
        {
            case SJF:
                add_SJF(t_ptr);
                break;
            case FCFS:
                add_FCFS(t_ptr);
                break;
        }
    }

    /**
     * Checks if the taskpool is empty
     * @return Boolean      True if empty, false if not empty
     */
    bool TaskPool::isEmpty()
    {
        // Use lock-guard for the mutex in the same way as a smart pointer
        // The mutex will be released when the lock-guard goes out of scope (end of function)
        std::lock_guard guard(mtx);
        return pool.size() <= 0;
    }

    /**
     * Gets the next task from the task pool and binds it to the provided pointer in the argument
     * @param task      Double pointer to store the reference to the task in.
     * @return Boolan   Returns false if the pool is empty and true if a task was bound to the given pointer
     */
    bool TaskPool::getNext(Task **task)
    {
        // Use lock-guard for the mutex in the same way as a smart pointer
        // The mutex will be released when the lock-guard goes out of scope (end of function)
        std::lock_guard guard(mtx);
        // Should use empty?
        if (pool.size() == 0)
            return false;

        // Get the first task
        *task = pool.begin()->second;
        // Remove the first task from the map
        pool.erase(pool.begin());
        return true;
    }

    TaskPool::TaskPool(TaskPool::SCHEDULING_POLICY policy) : policy(policy)
    {
    }

    void TaskPool::add_FCFS(Task *t_ptr)
    {
        pool[t_ptr->id] = t_ptr;
    }

    void TaskPool::add_SJF(Task *t_ptr)
    {
        /*
         * This can be optimized later by using insertion sort instead of sorting the whole vector again and again!
         */
        pool[t_ptr->estimatedExecutionTime] = t_ptr;
    }

    bool TaskPool::hasTask(int taskId)
    {
        std::lock_guard guard(mtx);

        // We iterate over all object because the key used depends on the scheduling policy and is not
        // always the taskid.
        for(auto poolPair : pool)
            if(poolPair.second->id == taskId)
                return true;
        return false;
    }
}
