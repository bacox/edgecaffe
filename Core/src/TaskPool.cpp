//
// Created by bacox on 26/03/2020.
//

#include "../include/TaskPool.h"
#include <algorithm>
#include <memory>
#include <string>
namespace EdgeCaffe
{

    class tmp_Data {
    public:
        int x = 4;
        double y = 8;
        std::string item = "Hello";
    };
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
        if (pool.size() <= 0)
            return false;

        *task = pool.front();
        pool.pop_front();
        return true;
    }

    TaskPool::TaskPool(TaskPool::SCHEDULING_POLICY policy) : policy(policy)
    {
    }

    void TaskPool::add_FCFS(Task *t_ptr)
    {
        pool.push_back(t_ptr);

        std::sort(pool.begin(), pool.end(), [ ]( const Task* lhs, const Task* rhs )
        {
            return (lhs->id) < (rhs->id);
        });
    }

    void TaskPool::add_SJF(Task *t_ptr)
    {
        /*
         * This can be optimized later by using insertion sort instead of sorting the whole vector again and again!
         */
        pool.push_back(t_ptr);
        // sort by estimatedExecutionTime, ascending
        // So shortest job first
        std::sort(pool.begin(), pool.end(), [ ]( const Task* lhs, const Task* rhs )
        {
            return (lhs->estimatedExecutionTime) < (rhs->estimatedExecutionTime);
        });
    }
}
