//
// Created by bacox on 24/07/2020.
//

#include "../../include/TaskPool/TypePriorityTaskPool.h"

void EdgeCaffe::TypePriorityTaskPool::addTask(EdgeCaffe::Task *t_ptr)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    /*
     * This can be optimized later by using insertion sort instead of sorting the whole vector again and again!
     */
    this->pool[PriorityKey{t_ptr->t_type, t_ptr->id}] = t_ptr;
}


