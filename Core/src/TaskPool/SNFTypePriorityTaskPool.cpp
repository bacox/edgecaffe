//
// Created by bacox on 24/07/2020.
//

#include "../../include/TaskPool/SNFTypePriorityTaskPool.h"

void EdgeCaffe::SNFTypePriorityTaskPool::addTask(EdgeCaffe::Task *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    /*
     * This can be optimized later by using insertion sort instead of sorting the whole vector again and again!
     */
    this->pool[SNFPriorityKey{t_ptr->networkExecutionTime, t_ptr->t_type, t_ptr->id}] = t_ptr;
}

bool EdgeCaffe::SNFTypePriorityTaskPool::hasTask(int taskId)
{

        // We iterate over all object because the key used depends on the scheduling policy and is not
        // always the taskid.
        for(auto poolPair : pool)
            if(poolPair.second->id == taskId)
                return true;
        return false;
}

bool EdgeCaffe::SNFTypePriorityTaskPool::isEmpty()
{
    return pool.empty();
}

bool EdgeCaffe::SNFTypePriorityTaskPool::getNext(EdgeCaffe::Task **task)
{
    if (pool.size() == 0)
        return false;
    auto it = pool.begin();
    *task = it->second;
    pool.erase(it);
    return true;
}
