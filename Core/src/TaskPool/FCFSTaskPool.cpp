//
// Created by bacox on 27/07/2020.
//

#include "../../include/TaskPool/FCFSTaskPool.h"

void EdgeCaffe::FCFSTaskPool::addTask(EdgeCaffe::Task *t_ptr)
{
//    // Use lock-guard for the mutex in the same way as a smart pointer
//    // The mutex will be released when the lock-guard goes out of scope (end of function)
//    std::lock_guard guard(mtx);
    /*
     * This can be optimized later by using insertion sort instead of sorting the whole vector again and again!
     */
    this->pool[t_ptr->id] = t_ptr;
}

bool EdgeCaffe::FCFSTaskPool::hasTask(int taskId)
{
    // We iterate over all object because the key used depends on the scheduling policy and is not
    // always the taskid.
    for(auto poolPair : pool)
        if(poolPair.second->id == taskId)
            return true;
    return false;
}

//EdgeCaffe::FCFSTaskPool::FCFSTaskPool(
//        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
//) : AbstractTaskPool(mc, nr)
//{}

bool EdgeCaffe::FCFSTaskPool::isEmpty()
{
    return pool.empty();
}

bool EdgeCaffe::FCFSTaskPool::getNext(EdgeCaffe::Task **task)
{
    if (pool.size() == 0)
        return false;
    auto it = pool.begin();
    *task = it->second;
    pool.erase(it);
    return true;
}
