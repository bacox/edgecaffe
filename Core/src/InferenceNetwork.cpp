//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceNetwork.h"
#include "../include/Tasks/LoadTask.h"
#include "../include/Tasks/ExecTask.h"
#include <yaml-cpp/yaml.h>
#include <opencv2/imgproc.hpp>
#include <cv.hpp>

InferenceNetwork::InferenceNetwork(const std::string &pathToDescription) : pathToDescription(pathToDescription) {}

void InferenceNetwork::init() {
    std::string pathToYaml = pathToDescription + "/description.yaml";
    YAML::Node description = YAML::LoadFile(pathToYaml);

    InferenceSubTask *sub = new InferenceSubTask;

    sub->networkName = description["name"].as<std::string>();

    std::string networkFile = description["network-file"].as<std::string>();
    std::string partialsLocation = description["partials-location"].as<std::string>();
    sub->basePath = description["base-path"].as<std::string>();
    sub->modelFileName = networkFile;
    sub->pathToModelFile = pathToDescription + "/" + networkFile;
    sub->pathToPartials = pathToDescription + "/" + partialsLocation;

    sub->pathToParamFile = pathToDescription + "/" + description["model-file"].as<std::string>();

    sub->hasInputLayer = description["has-input-layer"].as<bool>();
    sub->num_conv = description["num-conv-layers"].as<int>();
    sub->num_fc = description["num-fc-layers"].as<int>();
    sub->num_layers = sub->num_conv + sub->num_fc;
//    std::vector<std::string> resultVector = description["result-vector"].as<std::vector<std::string>>();
    auto iSize = description["input-scale"];
    sub->inputSize = cv::Size(iSize[0].as<int>(), iSize[1].as<int>());
    auto meanValues = description["mean-scalar"];
    sub->modelMeanValues = cv::Scalar(meanValues[0].as<float>(), meanValues[1].as<float>(), meanValues[2].as<float>());

    bool usesResultVector = description["uses-result-vector"].as<bool>();
    if(usesResultVector) {
        sub->resultVector = description["result-vector"].as<std::vector<std::string>>();
    }



    auto convLayers = description["conv-layers"].as<std::vector<std::string>>();
    auto fcLayers = description["fc-layers"].as<std::vector<std::string>>();

    for(std::string partialName : convLayers) {
        sub->partialNames.push_back(sub->pathToPartials + "/" + partialName);
    }

    for(std::string partialName : fcLayers) {
        sub->partialNames.push_back(sub->pathToPartials + "/" + partialName);
    }

    subTasks.push_back(sub);

}

void InferenceNetwork::setInput(cv::Mat &input, bool use_scales) {
    subTasks.front()->inputData = input;
    subTasks.front()->origInputData = input;
    preprocess(use_scales);
}

void InferenceNetwork::loadInputToNetwork() {
    InferenceSubTask *ptr = subTasks.front();
    std::vector<cv::Mat> channels;
    cv::split(ptr->inputData, channels);
    channels[0] -= ptr->modelMeanValues[0];
    channels[1] -= ptr->modelMeanValues[1];
    channels[2] -= ptr->modelMeanValues[2];

    OpenCV2Blob(channels, *(ptr->net_ptr));

}

void InferenceNetwork::loadNetworkStructure() {
    InferenceSubTask *ptr = subTasks.front();
    ptr->net_ptr = new caffe::Net<float>(ptr->pathToModelFile, caffe::Phase::TEST);
}

void InferenceNetwork::preprocess(bool use_scales) {
    InferenceSubTask *ptr = subTasks.front();
    if(use_scales)
    {
        // Fix later
    } else {
        cv::resize(ptr->inputData, ptr->inputData, ptr->inputSize);
    }
    ptr->inputData.convertTo(ptr->inputData, CV_32FC3);
}

void InferenceNetwork::createTasks() {
    int taskCounter = 0;

    InferenceSubTask *dnn = subTasks.front();
    int numLayers = dnn->net_ptr->layers().size();
    Task * lastTask = nullptr;

    bool hasInputLayer = dnn->hasInputLayer;
    bool firstLayer = true;
    int layerOffset = 0;
    if(!hasInputLayer) {
        layerOffset = -1;
    }
    for(int i = 0; i < numLayers; ++i)
    {
//        LoadTask t_l;
//        ExecTask t_e;
        std::cout << "This is layer " << i << std::endl;

        LoadTask *load = new LoadTask;

        load->network_ptr = dnn->net_ptr;
//        load->network_ptr = dnn->net_ptr;
        load->taskName = dnn->networkName + "-load-" + dnn->net_ptr->layer_names()[i];
        load->layerId = i + layerOffset;
        load->pathToPartial = dnn->partialNames[load->layerId];
//        load->layer = dnn->net_ptr->layers()[i].get();
        load->id = taskCounter++;

        if(firstLayer)
        {
            firstLayer = false;
            load->needsLoading = hasInputLayer;
        }

//        if(lastTask != nullptr) {
//            load->addTaskDependency(lastTask);
//        }
//        lastTask = load;

        tasks.push_back(load);


        ExecTask *exec = new ExecTask;

        exec->network_ptr = dnn->net_ptr;
//        exec->network_ptr = dnn->net_ptr;
        exec->taskName = dnn->networkName + "-exec-" + dnn->net_ptr->layer_names()[i];
        exec->layerId = i;
//        exec->layer = dnn->net_ptr->layers()[i].get();
        exec->id = taskCounter++;
        if(lastTask != nullptr) {
            exec->addTaskDependency(lastTask);
        }
        exec->addTaskDependency(load);
        lastTask = exec;
        tasks.push_back(exec);
    }

}

const std::vector<Task *> &InferenceNetwork::getTasks() const {
    return tasks;
}

void InferenceNetwork::showResult() {
        InferenceSubTask *ptr = subTasks.front();

        if(ptr->resultVector.size() == 0) {
            std::cout << "No resultvector to show results from " << std::endl;
            return;
        }

        caffe::Blob<float> *output_layer = ptr->net_ptr->output_blobs()[0];

        const float *begin = output_layer->cpu_data();
        const float *end = begin + output_layer->channels();
        std::vector<float> result = std::vector<float>(begin, end);

        auto it = max_element(std::begin(result), std::end(result));
        long pos = it - std::begin(result);
//
        std::string output = ptr->resultVector[pos];
//
        std::cout << "The output of " << ptr->networkName << " is: '" << output << "'" << std::endl;

        cv::imshow("Output: " + output, ptr->origInputData);
        cv::waitKey();

}

void InferenceNetwork::createTasks(int splittingPolicy) {
    std::string policyName = "";
    if(splittingPolicy == 0)
    {
        policyName = "Partial";
    } else if ( splittingPolicy == 1) {
        policyName = "conv-fc";
    } else {
        policyName = "Bulk";
    }
    std::cout << "Splitting policy is '" << policyName << "'" << std::endl;

    if(splittingPolicy == 0) // partial
    {
        createTasks();
    } else if ( splittingPolicy == 1) { // conv-fc
        createTasksConvFC();
    } else { // bulk
        createTasksBulk();
    }

}

void InferenceNetwork::createTasksConvFC() {\
    int taskCounter = 0;

    InferenceSubTask *dnn = subTasks.front();
    int numLayers = dnn->net_ptr->layers().size();

    int numConv = dnn->num_conv;
    int numFC = dnn->num_fc;
    Task * lastTask = nullptr;

    bool hasInputLayer = dnn->hasInputLayer;
    bool firstLayer = true;
    int layerOffset = 0;
    if(!hasInputLayer) {
        layerOffset = -1;
    }
    int poolId = 0;
    for(int i = 0; i < numLayers; ++i)
    {
        if(i < numConv)
        {
            poolId = 0;
        } else {
            poolId = 1;
        }
        std::cout << "This is layer " << i << std::endl;

        LoadTask *load = new LoadTask;

        load->network_ptr = dnn->net_ptr;
//        load->network_ptr = dnn->net_ptr;
        load->taskName = dnn->networkName + "-load-" + dnn->net_ptr->layer_names()[i];
        load->layerId = i + layerOffset;
        load->pathToPartial = dnn->partialNames[load->layerId];
//        load->layer = dnn->net_ptr->layers()[i].get();
        load->id = taskCounter++;

        if(firstLayer)
        {
            firstLayer = false;
            load->needsLoading = hasInputLayer;
        }

//        if(lastTask != nullptr) {
//            load->addTaskDependency(lastTask);
//        }
//        lastTask = load;

        load->assignedPoolId = poolId;

        tasks.push_back(load);


        ExecTask *exec = new ExecTask;

        exec->network_ptr = dnn->net_ptr;
//        exec->network_ptr = dnn->net_ptr;
        exec->taskName = dnn->networkName + "-exec-" + dnn->net_ptr->layer_names()[i];
        exec->layerId = i;
//        exec->layer = dnn->net_ptr->layers()[i].get();
        exec->id = taskCounter++;
        if(lastTask != nullptr) {
            exec->addTaskDependency(lastTask);
        }
        exec->addTaskDependency(load);
        exec->assignedPoolId = poolId;
        lastTask = exec;
        tasks.push_back(exec);
    }
}

void InferenceNetwork::createTasksBulk() {
    int taskCounter = 0;

    InferenceSubTask *dnn = subTasks.front();
    int numLayers = dnn->net_ptr->layers().size();
    Task * lastLoadTask = nullptr;
    Task * lastExecTask = nullptr;
    Task * firstExecTask = nullptr;

    bool hasInputLayer = dnn->hasInputLayer;
    bool firstLayer = true;
    int layerOffset = 0;
    if(!hasInputLayer) {
        layerOffset = -1;
    }
    for(int i = 0; i < numLayers; ++i)
    {
//        LoadTask t_l;
//        ExecTask t_e;
        std::cout << "This is layer " << i << std::endl;

        LoadTask *load = new LoadTask;

        load->network_ptr = dnn->net_ptr;
//        load->network_ptr = dnn->net_ptr;
        load->taskName = dnn->networkName + "-load-" + dnn->net_ptr->layer_names()[i];
        load->layerId = i + layerOffset;
        load->pathToPartial = dnn->partialNames[load->layerId];
//        load->layer = dnn->net_ptr->layers()[i].get();
        load->id = taskCounter++;

        if(firstLayer)
        {
            firstLayer = false;
            load->needsLoading = hasInputLayer;
        }

//        if(lastTask != nullptr) {
//            load->addTaskDependency(lastTask);
//        }
//        lastTask = load;

        if(lastLoadTask != nullptr)
        {
            load->addTaskDependency(lastLoadTask);
        }
        lastLoadTask = load;

        tasks.push_back(load);


        ExecTask *exec = new ExecTask;

        exec->network_ptr = dnn->net_ptr;
//        exec->network_ptr = dnn->net_ptr;
        exec->taskName = dnn->networkName + "-exec-" + dnn->net_ptr->layer_names()[i];
        exec->layerId = i;
//        exec->layer = dnn->net_ptr->layers()[i].get();
        exec->id = taskCounter++;

        if(firstExecTask == nullptr)
        {
            firstExecTask = exec;
        }

        if(lastExecTask != nullptr) {
            exec->addTaskDependency(lastExecTask);
        }
//        exec->addTaskDependency(load);
        lastExecTask = exec;
        tasks.push_back(exec);
    }
    if(firstExecTask != nullptr)
        firstExecTask->addTaskDependency(lastLoadTask);
}
