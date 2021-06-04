//
// Created by bacox on 24/07/2020.
//

#ifndef EDGECAFFE_ABSTRACTTASKPOOL_H
#define EDGECAFFE_ABSTRACTTASKPOOL_H

#include <mutex>
#include <map>
#include <any>
#include <boost/any.hpp>
#include "Tasks/Task.h"
#include "MemoryCounter.h"
#include "NetworkRegistry.h"

namespace EdgeCaffe {

    class AbstractTaskPoolBase {
    public:
        virtual ~AbstractTaskPoolBase() {};

        // Empty check
        virtual bool isEmpty() = 0;

        // Existence check, meant for debugging
        virtual bool hasTask(int taskId) = 0;

        // Insertion, should be implemented by concrete implementation
        virtual void addTask(Task *t_ptr) = 0;

        // Iterator implementation, should also perform existence check.
        virtual bool getNext(Task **task) = 0;
    };

    template<typename T>
    class AbstractTaskPool : public AbstractTaskPoolBase {

    public:

        std::map<T, Task *> pool;
        int poolId = -1;

        /**
         * @brief Add a reference of a task to the taskpool.
         * @param t_ptr     Task pointer
         */
        virtual void addTask(Task *t_ptr) = 0;

        /**
         * @brief Function to test for presence of task within the taskpool, based on checking TaskID of a Task in the
         * taskpool
         * @param taskId
         * @return Boolean indicating the presence of a task in the Taskpool.
         */
        bool hasTask(int taskId) {
            // We iterate over all object because the key used depends on the scheduling policy and is not
            // always the taskid.
#if __cplusplus >= 201103L
            // If C++ 11 or newer, we can benefit from the any functions
            return std::any_of(pool.begin(), pool.end(), [taskId](const std::pair<const T, Task *> &task) {
                return task.second->id == taskId;
            });
#else
            // Otherwise, we explicitly create and iterator and loop over the objects.
            for (std::pair<T, Task*> taskPair : pool) {
                if (taskPair.second->id == taskId) {
                    return true;
                }
            }
            return false;
#endif
        }

        /**
         * Checks if the taskpool is empty
         * @return Boolean      True if empty, false if not empty
         */
        bool isEmpty() {
            return pool.empty();
        }

        /**
         * Gets the next task from the task pool and binds it to the provided pointer in the argument
         * @param task      Double pointer to store the reference to the task in.
         * @return Boolean   Returns false if the pool is empty and true if a task was bound to the given pointer
         */
        bool getNext(EdgeCaffe::Task **task) {
            if (pool.empty())
                return false;
            auto it = pool.begin();
            *task = it->second;
            pool.erase(it);
            return true;
        }

    };
}
#endif //EDGECAFFE_ABSTRACTTASKPOOL_H
