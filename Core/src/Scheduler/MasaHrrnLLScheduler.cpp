//
// Created by bacox on 03/09/2020.
//

#include "../../include/Scheduler/MasaHrrnLLScheduler.h"

void EdgeCaffe::MasaHrrnLLScheduler::setElapsedTime(int elapsedTime)
{
    elapsed_time = elapsedTime;

}

void EdgeCaffe::MasaHrrnLLScheduler::addTask(std::shared_ptr<Task> t_ptr)
{
// Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    this->pool.emplace_back(HrrnItem(t_ptr->t_type, t_ptr->id, t_ptr->estimatedExecutionTime, t_ptr));
    updateTaskTime();
}

void EdgeCaffe::MasaHrrnLLScheduler::updateTime()
{
    for( auto&&  item : pool)
        item.updateRR(elapsed_time);
}

void EdgeCaffe::MasaHrrnLLScheduler::calcElapsedTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
//    const auto elapsed_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
    last_tp = now;
}

bool EdgeCaffe::MasaHrrnLLScheduler::isEmpty()
{
    return pool.size() == 0;
}

void EdgeCaffe::MasaHrrnLLScheduler::sortTasks()
{
    updateTaskTime();
    // Sort the pool of tasks
    pool.sort(std::greater<HrrnItem>());
//    std::sort(this->pool.begin(), this->pool.end(), std::greater<HrrnItem>());
}

void EdgeCaffe::MasaHrrnLLScheduler::updateTaskTime()
{
// Determine elapsed time
    calcElapsedTime();
    // Update all tasks
    updateTime();
}

bool EdgeCaffe::MasaHrrnLLScheduler::getNext(std::shared_ptr<Task> *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    // If the pool is tasks is empty stop here
    if(this->pool.size() == 0)
        return false;


//    *t_ptr = pool.back().t_ptr;
//    pool.pop_back();
//    pool.erase(pool.end());
//    return true;


    // Make sure all the tasks are sorted
    sortTasks();

//    auto it = this->pool.begin();

    bool isFirst = true;
    for(auto it = this->pool.begin() ; it!=pool.end(); ++it)
    {
        // Check if task is valid
        // Removed `!*masaEnabled || ` because masa should be enabled.
        if(this->nr->numActiveNetworks() == 0 || it->t_ptr->requiredMemory <= mc->getFreeSpace() || it->t_ptr->requiredMemory == 0)
        {
            *t_ptr = it->t_ptr;
            auto requiredMem = (*t_ptr)->requiredMemory;
            mc->lockMemory(requiredMem);
            std::swap(*it, pool.back());
            pool.pop_back();
//            if(isFirst)
//                pool.pop_front();
//            else
////                std::remove()
//                pool.erase(it);
            if((*t_ptr)->t_type == Task::INIT)
            {
                nr->activateNetwork();
            }
            return true;
        }
        isFirst = false;
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

EdgeCaffe::MasaHrrnLLScheduler::MasaHrrnLLScheduler(
        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
) : Scheduler(mc, nr)
{
    last_tp = std::chrono::high_resolution_clock::now();
}

const std::list<EdgeCaffe::MasaHrrnLLScheduler::HrrnItem> &EdgeCaffe::MasaHrrnLLScheduler::getPool() const
{
    return pool;
}
