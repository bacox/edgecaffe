//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_TASK_H
#define PIPELINE_TASK_H

#include <InferenceTask.h>
#include <caffe/net.hpp>
#include <utility>
#include <Profiler/NetworkProfiler.h>

namespace EdgeCaffe
{
    class TaskDependency;
    class ConditionalDependency;




//    class TaskReference {
//        bool executed = false;
//        Task *content = nullptr;
//    };

    class Task
    {
    public:

        enum TYPE {
            INIT= 2,
            LOAD= 1,
            EXEC= 0
        };

        enum LAYER_TYPE {
            CONV = 0,
            FC = 1
        };

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

        double networkExecutionTime = std::numeric_limits<double>::max();

        // Describes the task
        std::string taskName;

        // The layer id of the layer where the task belongs to
        int layerId = -1;

        std::string layerName = "";
        std::string networkName = "";
        std::string taskType = "";

        TYPE t_type;
        LAYER_TYPE layerType;

        bool saveResults{false};
        InferenceTask* itask= {nullptr};
        bool invalid = false;

        // Information for the scheduler (orchestrator) to use specific taskpools for this task if the poolId is set.
        int assignedPoolId = -1;

        // Holds references to the task that are dependencies for this task.
        // If the tasks in this are not executed, this current task is not ready to be run
//        std::vector<Task *> dependsOn;

        std::vector<TaskDependency> dependsOn;
        std::vector<ConditionalDependency> dependsOnConditional;

        // Condition to enforce linear behaviour for inter- network dependencies
        bool * dependencyCondition;
        double requiredMemory = 0;


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
        void saveNetworkResults();

        bool waitsForOtherTasks();

        [[nodiscard]] std::vector<TaskDependency> getDependencies() const;
        [[nodiscard]] std::vector<ConditionalDependency> getConditionalDependencies() const;

//        Task(int id, int executionTime);
//
//        Task();

        Task(int id, int networkId, const std::string &taskName, int estimatedExecutionTime = 0, int estimatedNeededMemory = 0);

        void addTaskDependency(TaskDependency t);
        void addTaskDependency(ConditionalDependency t);

        virtual ~Task();

        bool hasPoolAssigned();

        int getAssignedPoolId();
        void invalidate();
        bool isValid();

        virtual std::string getTaskDescription();

        void execute();

        static bool compByEstTime(const std::shared_ptr<Task> a, const std::shared_ptr<Task> b)
        {
            return (a->estimatedExecutionTime) < (b->estimatedExecutionTime);
        }

    protected:
//  Virtual function to implement in subclasses
        virtual void run() = 0;
    };


    class TaskDependency {
    public:
        std::shared_ptr<Task> dependency;

    public:
        explicit
        TaskDependency(std::shared_ptr<Task> dependency) : dependency(std::move(dependency))
        {}

        [[nodiscard]] bool isExecuted() const
        {
            if(dependency == nullptr)
                return true;
            return dependency->executed;
        }
    };

    class ConditionalDependency{
        std::shared_ptr<Task> dependency;
        bool * condition;
    public:
        ConditionalDependency(std::shared_ptr<Task> dependency, bool *condition) : dependency(dependency), condition(condition)
        {
        }

        [[nodiscard]] bool isExecuted() const
        {
            // Assume that the dependency is global and is always set by the system
            // If the condition is true means that the dependency should be enforces
            // If the condition is false means that the condition should be ignored
            if(!*condition)
                return true; // Ignore the actual value of the dependency because the condition if false
            // Dependency will be enforced, lets the dependency itself.
            if(dependency == nullptr)
                return true;
            return dependency->executed;
        }

        static bool compareByNetworkTime(const Task & l, const Task & r) //(2)
        {
            return l.networkExecutionTime < r.networkExecutionTime;
        }
    };
}

#endif //PIPELINE_TASK_H
