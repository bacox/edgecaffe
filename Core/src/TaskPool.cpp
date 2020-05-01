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
        pool.push_back(t_ptr);
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
        if (pool.size() <= 0)
            return false;
        *task = pool.front();
        pool.pop_front();
        return true;
    }
}
