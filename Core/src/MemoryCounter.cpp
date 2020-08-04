//
// Created by bacox on 03/07/2020.
//

#include <iostream>
#include "../include/MemoryCounter.h"

EdgeCaffe::MemoryCounter::MemoryCounter(double capacity) : capacity(capacity)
{}

double EdgeCaffe::MemoryCounter::getCapacity() const
{
    return capacity;
}

double EdgeCaffe::MemoryCounter::getFreeSpace() const
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    return freeSpace < 0 ? 0 : freeSpace;
}

double EdgeCaffe::MemoryCounter::getRealFreeSpace() const
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    return freeSpace;
}

void EdgeCaffe::MemoryCounter::lockMemory(double usage)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    freeSpace -= usage;
}

void EdgeCaffe::MemoryCounter::releaseMemory(double usage)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    freeSpace += usage;

}

void EdgeCaffe::MemoryCounter::setCapacity(double capacity)
{
    std::lock_guard guard(mtx);
    MemoryCounter::capacity = capacity;
    freeSpace = capacity;
}
