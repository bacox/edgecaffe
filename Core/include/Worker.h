//
// Created by bacox on 26/03/2020.
//

#ifndef PIPELINE_WORKER_H
#define PIPELINE_WORKER_H


#include "TaskPool.h"
#include <thread>
#include <Profiler/MemCheck.h>
#include <TaskPool/AbstractTaskPool.h>
#include <Scheduler/Scheduler.h>

namespace EdgeCaffe
{

    struct WorkerProfileLine {
        enum TYPE
        {
            IDLE,
            BUSY
        };
        TYPE type;
        std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> stop;
        long duration = 0;
        void calcDuration() {
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        }

        std::string toCsvLine(std::chrono::time_point<std::chrono::system_clock> ref) {
            auto startTime = std::chrono::duration_cast<std::chrono::microseconds>(start - ref).count();
            auto stopTime = std::chrono::duration_cast<std::chrono::microseconds>(stop - ref).count();
            std::string sep = ",";
            std::string t = (type == IDLE) ? "idle" : "busy";
            return std::to_string(startTime) + sep + std::to_string(stopTime) + sep + std::to_string(duration) + sep + t;
        }

    };

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

        bool verbose = true;
        #ifdef MEMORY_CHECK_ON
        // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        MemCheck *perf = nullptr;
        #endif

        // Taskpool holding the references to the ready tasks
        std::shared_ptr<TaskPool> outpool;

        // Taskpool holding the references to the finished tasks
//        TaskPool *pool;
//        std::shared_ptr<AbstractTaskPool> pool;
        std::shared_ptr<Scheduler> scheduler;

        virtual ~Worker();

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
//        Worker(TaskPool *pool, TaskPool *outpool);

        /**
         * Constructor
         * @param pool      TaskPool pointer
         * @param outpool   TaskPool pointer
         * @param workerId  Int - Assign a specific id to the worker
         */
//        Worker(TaskPool *pool, TaskPool *outpool, int workerId);

//        Worker(const std::shared_ptr<TaskPool> &outpool, const std::shared_ptr<AbstractTaskPool> &pool, int workerId);
        Worker(const std::shared_ptr<TaskPool> &outpool, const std::shared_ptr<Scheduler> &pool, int workerId);

        void measureIdleTime();
        void measureBusyTime();
        void startTimeMeasuring();
        void endTimeMeasuring();

        std::vector<std::string> workerProfileToCSVLines();

    private:
        std::chrono::time_point<std::chrono::system_clock> startTP;
        std::vector<WorkerProfileLine> workerProfile;
        WorkerProfileLine::TYPE workerStatus = WorkerProfileLine::IDLE;
    };
}

#endif //PIPELINE_WORKER_H
