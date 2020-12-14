//
// Created by bacox on 28/08/2020.
//

#include "../../include/Scheduler/MasaHrrnScheduler.h"
#include <algorithm>

void EdgeCaffe::MasaHrrnScheduler::addTask(EdgeCaffe::Task *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
//    this->pool.emplace_back(HrrnItem(t_ptr->t_type, t_ptr->id, t_ptr->estimatedExecutionTime, t_ptr));
    this->pool.push_back(t_ptr);
    updateTaskTime();
}

bool EdgeCaffe::MasaHrrnScheduler::isEmpty()
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    return this->pool.empty();
}

bool EdgeCaffe::MasaHrrnScheduler::getNext(EdgeCaffe::Task **t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    // If the pool is tasks is empty stop here
    if(this->pool.size() == 0)
        return false;


//    *t_ptr = pool.back().t_ptr;
//    pool.erase(pool.end());
//    pool.pop_back();
//    return true;


    // Make sure all the tasks are sorted
//    sortTasks();

//    auto it = this->pool.begin();

    for(auto it = this->pool.begin() ; it!=pool.end(); ++it)
    {
        // Check if task is valid
        // Removed `!*masaEnabled || ` because masa should be enabled.

//        if(this->nr->numActiveNetworks() == 0 || it->t_ptr->requiredMemory <= mc->getFreeSpace() || it->t_ptr->requiredMemory == 0)
        if(this->nr->numActiveNetworks() == 0 || (*it)->requiredMemory <= mc->getFreeSpace() || (*it)->requiredMemory == 0)
        {
            *t_ptr = *it;
            auto requiredMem = (*t_ptr)->requiredMemory;
            mc->lockMemory(requiredMem);

            std::swap(*it, pool.back());
            pool.pop_back();
//            pool.erase(it);

            if((*t_ptr)->t_type == Task::INIT)
            {
                nr->activateNetwork();
            }
            return true;
        }
    }
    return false;
//
//    *t_ptr = it->t_ptr;
//
//    this->pool.erase(it);
//
//    return true;


//    for(auto item: pool)
//        if(item.id == (*t_ptr)->id)
//            return true;
//    return false;
}

EdgeCaffe::MasaHrrnScheduler::MasaHrrnScheduler(
        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
) : Scheduler(mc, nr)
{
    pool.reserve(100000);
    last_tp = std::chrono::high_resolution_clock::now();
}

//const std::vector<EdgeCaffe::MasaHrrnScheduler::HrrnItem> &EdgeCaffe::MasaHrrnScheduler::getPool() const
const std::vector<EdgeCaffe::Task*> &EdgeCaffe::MasaHrrnScheduler::getPool() const
{
    return pool;
}

void EdgeCaffe::MasaHrrnScheduler::setElapsedTime(int elapsedTime)
{
    elapsed_time = elapsedTime;
}

void EdgeCaffe::MasaHrrnScheduler::updateTime()
{
//    for( auto&&  item : pool)
//        item.updateRR(elapsed_time);
}

void EdgeCaffe::MasaHrrnScheduler::calcElapsedTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
//    const auto elapsed_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
    last_tp = now;
}

void EdgeCaffe::MasaHrrnScheduler::sortTasks()
{
    updateTaskTime();
    // Sort the pool of tasks
//    std::sort(this->pool.begin(), this->pool.end(), std::greater<HrrnItem>());
    std::sort(this->pool.begin(), this->pool.end(), std::greater<EdgeCaffe::Task*>());
}

void EdgeCaffe::MasaHrrnScheduler::updateTaskTime()
{
    // Determine elapsed time
    calcElapsedTime();
    // Update all tasks
    updateTime();
}
