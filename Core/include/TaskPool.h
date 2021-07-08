//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_TASKPOOL_H
#define PIPELINE_TASKPOOL_H


#include <mutex>
#include <deque>
#include <map>
#include <Tasks/Task.h>
#include "MemoryCounter.h"
#include "NetworkRegistry.h"

namespace EdgeCaffe
{
    class TaskPool
    {
        /**
         * The task pool holds references to a set of tasks.
         * Some pools are available at multiple core and/or threads.
         * No actual data is held or managed in taskpools, it only holds pointers to tasks.
         */
    public:
        enum SCHEDULING_POLICY {
            FCFS,
            SJF
        };
        TaskPool(SCHEDULING_POLICY policy = FCFS);
    protected:
        std::mutex mtx;
    private:
        SCHEDULING_POLICY policy;
        void add_FCFS(std::shared_ptr<Task> t_ptr);

        void add_SJF(std::shared_ptr<Task> t_ptr);
    public:

        MemoryCounter *mc = nullptr;
        NetworkRegistry *nr = nullptr;
        bool *masaEnabled = nullptr;

        /**
         * Checks whethter a specific task is present in the pool
         * @param taskId
         * @return Boolean      True is present, false is not present in the pool
         */
        virtual bool hasTask(int taskId);

        int poolId = -1;
        /**
         * We use a map as the datastructure to hold the tasks.
         * std::maps are sorted by the used key so we can change the ordering by changing the key in the map.
         * Map is chosen in favor of deque because std::map doesn't need std::sort.
         *
         * std::sort causes memory corruption in the specifc case and should therefor not be used.
         * Use use pointers for taks to prevent copying of memory.
         */
        std::map<int, std::shared_ptr<Task>> pool;

        /**
         * Add a reference of a task to the taskpool
         * @param t_ptr     Task pointer
         */
        virtual void addTask(std::shared_ptr<Task> t_ptr);

        /**
         * Checks if the taskpool is empty
         * @return Boolean      True if empty, false if not empty
         */
        virtual bool isEmpty();

        /**
         * Gets the next task from the task pool and binds it to the provided pointer in the argument
         * @param task      Double pointer to store the reference to the task in.
         * @return Boolan   Returns false if the pool is empty and true if a task was bound to the given pointer
         */
        virtual bool getNext(std::shared_ptr<Task> *task);
    };
}


#endif //PIPELINE_TASKPOOL_H
