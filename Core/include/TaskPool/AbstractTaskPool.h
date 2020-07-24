//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_ABSTRACTTASKPOOL_H
#define EDGECAFFE_ABSTRACTTASKPOOL_H

#include <mutex>
#include <map>
#include "Tasks/Task.h"
#include "MemoryCounter.h"
#include "NetworkRegistry.h"

namespace EdgeCaffe {
    template<typename T>
    class AbstractTaskPool {
    protected:
        std::mutex mtx;

    public:
        MemoryCounter *mc = nullptr;
        NetworkRegistry *nr = nullptr;

        int poolId = -1;

        /**
         * We use a map as the datastructure to hold the tasks.
         * std::maps are sorted by the used key so we can change the ordering by changing the key in the map.
         * Map is chosen in favor of deque because std::map doesn't need std::sort.
         *
         * std::sort causes memory corruption in the specifc case and should therefor not be used.
         * Use use pointers for tasks to prevent copying of memory.
         */
        std::map<T, Task*> pool;

        virtual bool hasTask(int taskId)
        {
            std::lock_guard guard(mtx);

            // We iterate over all object because the key used depends on the scheduling policy and is not
            // always the taskid.
            for(auto poolPair : pool)
                if(poolPair.second->id == taskId)
                    return true;
            return false;
        }

        /**
         * Add a reference of a task to the taskpool
         * @param t_ptr     Task pointer
         */
        virtual void addTask(Task *t_ptr) = 0;

        /**
         * Checks if the taskpool is empty
         * @return Boolean      True if empty, false if not empty
         */
        virtual bool isEmpty()
        {
            // Use lock-guard for the mutex in the same way as a smart pointer
            // The mutex will be released when the lock-guard goes out of scope (end of function)
            std::lock_guard guard(mtx);
            return pool.empty();
        }

        /**
         * Gets the next task from the task pool and binds it to the provided pointer in the argument
         * @param task      Double pointer to store the reference to the task in.
         * @return Boolan   Returns false if the pool is empty and true if a task was bound to the given pointer
         */
//        virtual bool getNext(Task **task) = 0;
    };
}
#endif //EDGECAFFE_ABSTRACTTASKPOOL_H
