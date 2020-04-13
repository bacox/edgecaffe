//
// Created by bacox on 30/03/2020.
//

#ifndef PIPELINE_TASK_H
#define PIPELINE_TASK_H

#include <caffe/net.hpp>
#include <Profiler/NetworkProfiler.h>

class Task {
public:
//    InferenceSubTask *network;
    caffe::Net<float> *network_ptr;
//    caffe::Layer<float> *layer;
//    std::shared_ptr<GenericDNN> network;
    bool executed = false;
    int id = 0;
    int executionTime = 0;
    std::string taskName;
    int layerId;
    int assignedPoolId = -1;
    std::vector<Task*> dependsOn;

    ProfileLine profileLine;

    bool waitsForOtherTasks();

    std::vector<Task*> getDependencies();

    Task(int id, int executionTime);
    Task();

    void addTaskDependency(Task * t);

    virtual ~Task();

    bool hasPoolAssigned();

    int getAssignedPoolId();

    virtual std::string getTaskDescription();

    void execute();

protected:
//  Virtual function to implement in subclasses
    virtual void run() = 0;
};


#endif //PIPELINE_TASK_H
