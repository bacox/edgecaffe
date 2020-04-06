//
// Created by bacox on 26/03/2020.
//

#include "../include/TaskPool.h"

void TaskPool::addTask(Task* t_ptr) {
    std::lock_guard guard(mtx);
    pool.push_back(t_ptr);
}

bool TaskPool::isEmpty() {
    std::lock_guard guard(mtx);
    return pool.size() <= 0;
}
Task* TaskPool::getNext() {
    std::lock_guard guard(mtx);
    Task* ptr = pool.front();
    pool.pop_front();
    return ptr;
}

bool TaskPool::getNext(Task **task) {
    std::lock_guard guard(mtx);
    if(pool.size() <= 0)
        return false;
    *task = pool.front();
    pool.pop_front();
    return true;
}
