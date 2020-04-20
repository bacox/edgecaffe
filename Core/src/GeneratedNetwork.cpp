//
// Created by bacox on 19/04/2020.
//

#include <Tasks/DummyTask.h>
#include "GeneratedNetwork.h"

EdgeCaffe::GeneratedNetwork::GeneratedNetwork(const std::string &pathToDescription) : InferenceNetwork(
        pathToDescription
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

void EdgeCaffe::GeneratedNetwork::createTasks(int splittingPolicy)
{
    InferenceSubTask *dnn = subTasks.front();
    int numberOfLayers  = dnn->num_conv + dnn->num_fc;
    Task *lastTask = nullptr;
    for(auto layer : layers)
    {
        DummyTask *loading = new DummyTask(TASKID_COUNTER++, layer.loadingDuration);
        loading->layerId = layer.id;
        loading->taskName = dnn->networkName + "-dummy-loading-layer" + std::to_string(loading->layerId);
        loading->isLoadingTask = true;
        if(lastTask != nullptr)
            loading->dependsOn.push_back(lastTask);
        lastTask = loading;

        if (dnn->firstTask == nullptr)
        {
            dnn->firstTask = loading;
        }
        tasks.push_back(loading);

        DummyTask *exec = new DummyTask(TASKID_COUNTER++, layer.executionDuration);
        exec->layerId = layer.id;
        exec->taskName = dnn->networkName + "-dummy-executing-layer" + std::to_string(exec->layerId);
        if(lastTask != nullptr)
            exec->dependsOn.push_back(lastTask);
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
    std::cout << "Deallocating GeneratedNetwork" << std::endl;
}
