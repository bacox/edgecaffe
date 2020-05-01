//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_WORKER_H
#define PIPELINE_WORKER_H


#include "TaskPool.h"
#include <thread>

namespace EdgeCaffe
{
    class Worker
    {
        /**
         * Workers processes individual tasks.
         * A worker is running in its own thread and is running indepently
         * A worker is connected to two taskpools: the `pool` and the `outpool`
         * The `pool` taskpool holds all the ready tasks that the worker can execute.
         * The `outpool` taskpool holds all the finished tasks.
         * A worker will pick a task from the `pool` taskpool, executes it and put it in the `outpool` taskpool.
         */
    public:
        bool action_stop = false;
        // Flag used to indicate to the worker that he is allowed to stop if there are no more tasks left
        bool allowed_to_stop = false;
        std::thread _thread;

        // Taskpool holding the references to the ready tasks
        TaskPool *outpool;

        // Taskpool holding the references to the finished tasks
        TaskPool *pool;

        int workerId = -1;

        /**
         * Prints a message containing the worker id.
         * Used for debugging
         */
        void printMessage();

        /**
         * Main function of the threaded worker.
         * This will run continously during execution.
         * The worker checks if a task is available in the ready pool (`pool` variable)
         * If a task is available, the worker will:
         *  1. Retrieve the task
         *  2. Execute the task
         *  3. Move the task to the finished pool
         *  4. Set the execution property of the task to true
         */
        void Execution();

        /**
         * Start the thread of the worker
         */
        void run();

        /**
         * Checks if the worker may stop its thread
         * @return Boolean
         */
        bool AllowedToStop();

        /**
         * Constructor
         * @param pool      TaskPool pointer
         * @param outpool   TaskPool pointer
         */
        Worker(TaskPool *pool, TaskPool *outpool);

        /**
         * Constructor
         * @param pool      TaskPool pointer
         * @param outpool   TaskPool pointer
         * @param workerId  Int - Assign a specific id to the worker
         */
        Worker(TaskPool *pool, TaskPool *outpool, int workerId);
    };
}

#endif //PIPELINE_WORKER_H
