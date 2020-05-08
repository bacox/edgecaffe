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

        caffe::Net<float> *network_ptr;
        bool executed = false;
        int id = 0;
        int estimatedExecutionTime = 0;
        std::string taskName;
        int layerId;
        int assignedPoolId = -1;
        std::vector<Task *> dependsOn;

        // To measure the actual execution time
        ProfileLine profileLine;

        /*
         * For profiling more timing points we maybe need more time points
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

        Task(int id, int executionTime);

        Task();

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
