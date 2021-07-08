//
// Created by bacox on 19/04/2020.
//

#include <Tasks/DummyTask.h>
#include "GeneratedNetwork.h"

EdgeCaffe::GeneratedNetwork::GeneratedNetwork(const std::string &pathToDescription, bool* dependencyCondition) : InferenceNetwork(
        pathToDescription, dependencyCondition
)
{}

void EdgeCaffe::GeneratedNetwork::init(YAML::Node &description)
{
//    InferenceNetwork::init(description);
    InferenceSubTask *sub = new InferenceSubTask;
    sub->networkName = description["name"].as<std::string>();
    sub->num_conv = description["num-conv-layers"].as<int>();
    sub->num_fc = description["num-fc-layers"].as<int>();
    subTasks.push_back(sub);
    for( YAML::Node l : description["layers"])
    {
        DummyLayer layer;
        layer.name = l["name"].as<std::string>("");
        layer.loadingDuration = l["loading-duration"].as<long>(0);
        layer.executionDuration = l["execution-duration"].as<long>(0);
        layer.id = l["id"].as<int>(0);
        layer.dependsOn = l["dependsOn"].as<std::vector<int>>(std::vector<int>());
        layers.push_back(layer);
    }
}

void EdgeCaffe::GeneratedNetwork::createTasks(Type::MODE_TYPE splittingPolicy)
{
    InferenceSubTask *dnn = subTasks.front();
    int numberOfLayers  = dnn->num_conv + dnn->num_fc;
    std::shared_ptr<Task> lastTask = nullptr;

    for(auto layer : layers)
    {
        // Dummytask needs a taskId, networkId, and a description
//        DummyTask *loading = new DummyTask(TASKID_COUNTER++, -1, "dummy-loading", layer.loadingDuration);
        std::shared_ptr<DummyTask> loading = std::make_shared<DummyTask>(TASKID_COUNTER++, -1, "dummy-loading", layer.loadingDuration);
        loading->layerId = layer.id;
        loading->taskName = dnn->networkName + "-dummy-loading-layer" + std::to_string(loading->layerId);
        loading->isLoadingTask = true;
        if(lastTask != nullptr)
            loading->dependsOn.push_back(TaskDependency(lastTask));
        lastTask = loading;

        if (dnn->firstTask == nullptr)
        {
            dnn->firstTask = loading;
        }
        tasks.push_back(loading);


        std::shared_ptr<DummyTask> exec = std::make_shared<DummyTask>(TASKID_COUNTER++, -1, "dummy-loading", layer.executionDuration);
//        DummyTask *exec = new DummyTask(TASKID_COUNTER++, -1, "dummy-loading", layer.executionDuration);
        exec->layerId = layer.id;
        exec->taskName = dnn->networkName + "-dummy-executing-layer" + std::to_string(exec->layerId);
        if(lastTask != nullptr)
            exec->dependsOn.push_back(TaskDependency(lastTask));
        lastTask = exec;
        if (dnn->firstTask == nullptr)
        {
            dnn->firstTask = loading;
        }
        tasks.push_back(exec);
    }
    dnn->lastTask = lastTask;
}

EdgeCaffe::GeneratedNetwork::~GeneratedNetwork()
{
//    std::cout << "Deallocating GeneratedNetwork" << std::endl;
}
