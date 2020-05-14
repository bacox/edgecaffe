//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_TASK_H
#define PIPELINE_TASK_H

#include <caffe/net.hpp>
#include <Profiler/NetworkProfiler.h>

namespace EdgeCaffe
{
    class Task
    {
    public:
        enum TIME {
//            NET_SUBMIT,
            TO_WAITING,
            TO_READY,
            START,
            STOP,
            FINISHED
        };
        // Pointer to caffe network to have access to the deep neural net
        // Note: this is a double pointer so the network can be initialized after all the tasks are defined
        caffe::Net<float> **network_ptr;

        // Flag to tell the scheduler that the task is finished
        bool executed = false;

        // The id of the task; this should be a unique value
        int id = 0;
        // Id of the network it belongs to
        int networkId = 0;

        // The estimated time needed to execute this task. Can be used for scheduling
        int estimatedExecutionTime = 0;
        // The estimated memory needed to execution of this task. Can be used for scheduling
        int estimatedNeededMemory = 0;

        // Describes the task
        std::string taskName;

        // The layer id of the layer where the task belongs to
        int layerId;

        // Information for the scheduler (orchestrator) to use specific taskpools for this task if the poolId is set.
        int assignedPoolId = -1;

        // Holds references to the task that are dependencies for this task.
        // If the tasks in this are not executed, this current task is not ready to be run
        std::vector<Task *> dependsOn;

        // To measure the actual execution time
        ProfileLine profileLine;

        /*
         * For profiling more timing points we maybe need more time points
         * Get time snapshots for starting and ending events.
         */
//        std::chrono::time_point<std::chrono::system_clock> networkSubmission;
        std::chrono::time_point<std::chrono::system_clock> moveToWaiting;
        std::chrono::time_point<std::chrono::system_clock> moveToReady;
        std::chrono::time_point<std::chrono::system_clock> startTask;
        std::chrono::time_point<std::chrono::system_clock> stopTask;
        std::chrono::time_point<std::chrono::system_clock> networkFinished;

        void measureTime(TIME type);


        bool waitsForOtherTasks();

        std::vector<Task *> getDependencies();

//        Task(int id, int executionTime);
//
//        Task();

        Task(int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0, int estimatedNeededMemory = 0);

        void addTaskDependency(Task *t);

        virtual ~Task();

        bool hasPoolAssigned();

        int getAssignedPoolId();

        virtual std::string getTaskDescription();

        void execute();

        static bool compByEstTime(const Task* a, const Task *b)
        {
            return (a->estimatedExecutionTime) < (b->estimatedExecutionTime);
        }

    protected:
//  Virtual function to implement in subclasses
        virtual void run() = 0;
    };
}

#endif //PIPELINE_TASK_H
