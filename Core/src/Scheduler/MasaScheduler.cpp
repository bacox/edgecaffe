//
// Created by bacox on 27/07/2020.
//

#include "../../include/Scheduler/MasaScheduler.h"

void EdgeCaffe::MasaScheduler::addTask(Task *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    if( t_ptr->t_type == Task::EXEC)
        execTasks.addTask(t_ptr);
    else
        loadTasks.addTask(t_ptr);
}

bool EdgeCaffe::MasaScheduler::getNext(Task **t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    // Should use empty?
    if(execTasks.pool.size() > 0)
    {
        // Get the first exec task
        *t_ptr = execTasks.pool.begin()->second;
        // Remove the first task from the map
        execTasks.pool.erase(execTasks.pool.begin());
        return true;
    }


    auto it = loadTasks.pool.begin();
    for(it ; it!=loadTasks.pool.end(); ++it)
    {
        // Check if task is valid
        // Removed `!*masaEnabled || ` because masa should be enabled.
        if(this->nr->numActiveNetworks() == 0 || it->second->requiredMemory <= mc->getFreeSpace() || it->second->requiredMemory == 0)
        {
            *t_ptr = it->second;
            mc->lockMemory((*t_ptr)->requiredMemory);
            loadTasks.pool.erase(it);
            if((*t_ptr)->taskType == "init")
            {
                nr->activateNetwork();
            }
            return true;
        }
    }
    return false;
}

bool EdgeCaffe::MasaScheduler::isEmpty()
{
    std::lock_guard guard(mtx);
    return execTasks.isEmpty() || loadTasks.isEmpty();
}

EdgeCaffe::MasaScheduler::MasaScheduler(
        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
) : Scheduler(mc, nr)
{}
