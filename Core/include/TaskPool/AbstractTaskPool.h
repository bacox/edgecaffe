//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_ABSTRACTTASKPOOL_H
#define EDGECAFFE_ABSTRACTTASKPOOL_H

#include <mutex>
#include <map>
//#include "Tasks/Task.h"
#include "MemoryCounter.h"
#include "NetworkRegistry.h"
#include <memory>


namespace EdgeCaffe {
    class Task;
//    template<typename T>
    class AbstractTaskPool {

    public:
//        AbstractTaskPool(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr) : mc(mc),
//                                                                                                                 nr(nr)
//        {}

//        std::shared_ptr<MemoryCounter> mc;
//        std::shared_ptr<NetworkRegistry> nr;



        int poolId = -1;

        /**
         * We use a map as the datastructure to hold the tasks.
         * std::maps are sorted by the used key so we can change the ordering by changing the key in the map.
         * Map is chosen in favor of deque because std::map doesn't need std::sort.
         *
         * std::sort causes memory corruption in the specifc case and should therefor not be used.
         * Use use pointers for tasks to prevent copying of memory.
         */
//        std::map<T, Task*> pool;

        virtual bool hasTask(int taskId) = 0;

        /**
         * Add a reference of a task to the taskpool
         * @param t_ptr     Task pointer
         */
        virtual void addTask(std::shared_ptr<Task> t_ptr) = 0;

        /**
         * Checks if the taskpool is empty
         * @return Boolean      True if empty, false if not empty
         */
        virtual bool isEmpty() = 0;
//        {
//            // Use lock-guard for the mutex in the same way as a smart pointer
//            // The mutex will be released when the lock-guard goes out of scope (end of function)
////            std::lock_guard guard(mtx);
//            return pool.empty();
//        }

        /**
         * Gets the next task from the task pool and binds it to the provided pointer in the argument
         * @param task      Double pointer to store the reference to the task in.
         * @return Boolan   Returns false if the pool is empty and true if a task was bound to the given pointer
         */
        virtual bool getNext(std::shared_ptr<Task> *task) = 0;
//        {
//            if (pool.size() == 0)
//                return false;
//            auto it = pool.begin();
//            *task = it->second;
//            pool.erase(it);
//            return true;
//        }
    };
}
#endif //EDGECAFFE_ABSTRACTTASKPOOL_H
