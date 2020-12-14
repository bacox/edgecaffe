//
// Created by bacox on 13/06/2020.
//

#include "../include/PriorityTaskPool.h"

EdgeCaffe::PriorityTaskPool::PriorityTaskPool()
{}

bool EdgeCaffe::PriorityTaskPool::hasTask(int taskId)
{
    std::lock_guard guard(mtx);
    // We iterate over all object because the key used depends on the scheduling policy and is not
    // always the taskid.
    for(auto poolPair : execTasks)
        if(poolPair.second->id == taskId)
            return true;

        
    for(auto poolPair : loadTasks)
        if(poolPair.second->id == taskId)
            return true;

    return false;
}

void EdgeCaffe::PriorityTaskPool::addTask(EdgeCaffe::Task *t_ptr)
{
    std::lock_guard guard(mtx);
    if(t_ptr->taskType == "exec")
        execTasks[t_ptr->id] = t_ptr;
    else
        loadTasks[t_ptr->id] = t_ptr;
}

bool EdgeCaffe::PriorityTaskPool::isEmpty()
{
    std::lock_guard guard(mtx);
    return loadTasks.size() <= 0 && execTasks.size() <= 0;
}

bool EdgeCaffe::PriorityTaskPool::getNext(EdgeCaffe::Task **task)
{
    std::lock_guard guard(mtx);
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    // Should use empty?
    if(execTasks.size() > 0)
    {
        // Get the first exec task
        *task = execTasks.begin()->second;
        // Remove the first task from the map
        execTasks.erase(execTasks.begin());
        return true;
    }


    auto it = loadTasks.begin();
    for(it ; it!=loadTasks.end(); ++it)
    {
        // Check if task is valid
        if(!*masaEnabled || this->nr->numActiveNetworks() == 0 || it->second->requiredMemory <= mc->getFreeSpace() || it->second->requiredMemory == 0)
        {
            *task = it->second;
            mc->lockMemory((*task)->requiredMemory);
            loadTasks.erase(it);
            if((*task)->taskType == "init")
            {
                nr->activateNetwork();
            }
            return true;
        }
    }
    return false;
}
