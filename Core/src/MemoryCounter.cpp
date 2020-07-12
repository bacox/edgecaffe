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
//    std::cout << "[Load before] Memory Usage: " << (freeSpace < 0 ? 0 : freeSpace) << " inter-network enforcement? " << *interNetworkCondition << std::endl;
    freeSpace -= usage;
    if(freeSpace < 155.87)
    {
        *interNetworkCondition = true;
    }
    else
    {
        *interNetworkCondition = false;
    }

//    std::cout << "[Load after] Memory Usage: " << (freeSpace < 0 ? 0 : freeSpace) << " inter-network enforcement? " << *interNetworkCondition << std::endl;
}

void EdgeCaffe::MemoryCounter::releaseMemory(double usage)
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
//    std::cout << "[Unload before] Memory Usage: " << freeSpace << " inter-network enforcement? " << *interNetworkCondition << std::endl;
    freeSpace += usage;
    if(freeSpace < 155.87)
    {
        *interNetworkCondition = true;
    }
    else
    {
        *interNetworkCondition = false;
    }
//    std::cout << "[Unload after] Memory Usage: " << (freeSpace < 0 ? 0 : freeSpace) << " inter-network enforcement? " << *interNetworkCondition << std::endl;

}

void EdgeCaffe::MemoryCounter::setCapacity(double capacity)
{
    std::lock_guard guard(mtx);
    MemoryCounter::capacity = capacity;
    freeSpace = capacity;
}
