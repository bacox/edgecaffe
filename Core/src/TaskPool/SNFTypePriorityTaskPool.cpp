//
// Created by bacox on 24/07/2020.
//

#include "../../include/TaskPool/SNFTypePriorityTaskPool.h"

void EdgeCaffe::SNFTypePriorityTaskPool::addTask(EdgeCaffe::Task *t_ptr)
{
    this->pool[SNFPriorityKey{t_ptr->networkExecutionTime, t_ptr->t_type, t_ptr->id}] = t_ptr;
}