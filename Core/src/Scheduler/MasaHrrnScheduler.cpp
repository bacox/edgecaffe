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
    this->pool.emplace_back(HrrnItem(t_ptr->t_type, t_ptr->id, t_ptr->estimatedExecutionTime, t_ptr));
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

    if(this->pool.size() == 0)
        return false;

    sortTasks();

    auto it = this->pool.begin();

    *t_ptr = it->t_ptr;

    this->pool.erase(it);

    return true;


//    for(auto item: pool)
//        if(item.id == (*t_ptr)->id)
//            return true;
//    return false;
}

EdgeCaffe::MasaHrrnScheduler::MasaHrrnScheduler(
        const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr
) : Scheduler(mc, nr)
{
    last_tp = std::chrono::high_resolution_clock::now();
}

const std::vector<EdgeCaffe::MasaHrrnScheduler::HrrnItem> &EdgeCaffe::MasaHrrnScheduler::getPool() const
{
    return pool;
}

void EdgeCaffe::MasaHrrnScheduler::setElapsedTime(int elapsedTime)
{
    elapsed_time = elapsedTime;
}

void EdgeCaffe::MasaHrrnScheduler::updateTime()
{
    for( auto&&  item : pool)
        item.updateRR(elapsed_time);
}

void EdgeCaffe::MasaHrrnScheduler::calcElapsedTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
    const auto elapsed_time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tp).count();
    last_tp = now;
}

void EdgeCaffe::MasaHrrnScheduler::sortTasks()
{
    // Determine elapsed time
    calcElapsedTime();
    // Update all tasks
    updateTime();
    // Sort the pool of tasks
    std::sort(this->pool.begin(), this->pool.end(), std::greater<HrrnItem>());
}
