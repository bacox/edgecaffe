//
// Created by bacox on 27/07/2020.
//

#include <Tasks/Task.h>
#include "../../include/Scheduler/FCFSScheduler.h"

void EdgeCaffe::FCFSScheduler::addTask(Task *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    taskPool.addTask(t_ptr);
}

bool EdgeCaffe::FCFSScheduler::getNext(Task **t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    bool foundTask = taskPool.getNext(t_ptr);

    if(foundTask && (*t_ptr)->taskType == "init")
    {
        nr->activateNetwork();
    }

    return foundTask;
}

bool EdgeCaffe::FCFSScheduler::isEmpty()
{
    std::lock_guard guard(mtx);
    return taskPool.isEmpty();
}

EdgeCaffe::FCFSScheduler::FCFSScheduler(
        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
) : Scheduler(mc, nr)
{}