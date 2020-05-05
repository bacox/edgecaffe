//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceNetwork.h"
#include "../include/Tasks/LoadTask.h"
#include "../include/Tasks/ExecTask.h"
#include <yaml-cpp/yaml.h>
#include <opencv2/imgproc.hpp>
#include <cv.hpp>
#include <BaseNet.h>


namespace EdgeCaffe
{
    InferenceNetwork::InferenceNetwork(const std::string &pathToDescription) : pathToDescription(pathToDescription)
    {}

    int InferenceNetwork::TASKID_COUNTER = 0;

    void InferenceNetwork::init(YAML::Node &description)
    {
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
        sub->modelMeanValues = cv::Scalar(
                meanValues[0].as<float>(), meanValues[1].as<float>(), meanValues[2].as<float>());

        bool usesResultVector = description["uses-result-vector"].as<bool>();
        if (usesResultVector)
        {
            sub->resultVector = description["result-vector"].as<std::vector<std::string>>();
        }



        for(auto l_layer : description["conv-layers"])
        {
            layerDescriptions.push_back(LayerDescription::FromYaml(l_layer, sub->pathToPartials));
        }

        for(auto l_layer : description["fc-layers"])
        {
            layerDescriptions.push_back(LayerDescription::FromYaml(l_layer, sub->pathToPartials));
        }

        for( auto l_layer : layerDescriptions)
        {
            sub->partialNames.push_back(sub->pathToPartials + "/" + l_layer.partialFileName);
        }

//        std::cout << "Compare sizes " << sub->net_ptr->layers().size() << " && " << layerDescriptions.size() << std::endl;

//        auto convLayers = description["conv-layers"].as<std::vector<std::string>>();
//        auto fcLayers = description["fc-layers"].as<std::vector<std::string>>();
//
//        for (std::string partialName : convLayers)
//        {
//            sub->partialNames.push_back(sub->pathToPartials + "/" + partialName);
//        }
//
//        for (std::string partialName : fcLayers)
//        {
//            sub->partialNames.push_back(sub->pathToPartials + "/" + partialName);
//        }

        subTasks.push_back(sub);
    }
    void InferenceNetwork::init()
    {
        std::string pathToYaml = pathToDescription + "/description.yaml";
        YAML::Node description;
        try{
            description = YAML::LoadFile(pathToYaml);
        } catch(...){
//            std::exception_ptr p = std::current_exception();
            std::cerr << "Error while attempting to read yaml file!" << std::endl;
            std::cerr << "Yaml file: " << pathToYaml << std::endl;
        }
        init(description);
    }

    void InferenceNetwork::setInput(cv::Mat &input, bool use_scales)
    {
        subTasks.front()->inputData = input;
        subTasks.front()->origInputData = input;
        preprocess(use_scales);
    }

    void InferenceNetwork::loadInputToNetwork()
    {
        InferenceSubTask *ptr = subTasks.front();
        std::vector<cv::Mat> channels;
        cv::split(ptr->inputData, channels);
        channels[0] -= ptr->modelMeanValues[0];
        channels[1] -= ptr->modelMeanValues[1];
        channels[2] -= ptr->modelMeanValues[2];

        OpenCV2Blob(channels, *(ptr->net_ptr));
    }

    void InferenceNetwork::loadNetworkStructure()
    {
        InferenceSubTask *ptr = subTasks.front();
        ptr->net_ptr = new caffe::Net<float>(ptr->pathToModelFile, caffe::Phase::TEST);
    }

    void InferenceNetwork::preprocess(bool use_scales)
    {
        InferenceSubTask *ptr = subTasks.front();
        if (use_scales)
        {
            // Temporary fix
            std::vector<double> scales(scale_list(ptr->inputData));
            cv::resize(
                    ptr->inputData, ptr->inputData, cv::Size(
                            ptr->inputData.cols * scales[0], ptr->inputData.rows * scales[0]
                    ));
            ptr->pathToModelFile = updatePrototxt(
                    ptr->inputData.rows, ptr->inputData.cols, ptr->basePath, ptr->modelFileName
            );

        } else
        {
            cv::resize(ptr->inputData, ptr->inputData, ptr->inputSize);
        }
        ptr->inputData.convertTo(ptr->inputData, CV_32FC3);
    }

    void InferenceNetwork::createTasks()
    {
        InferenceSubTask *dnn = subTasks.front();
        int numLayers = dnn->net_ptr->layers().size();
        Task *lastTask = nullptr;
        for (int i = 0; i < numLayers; ++i)
        {

            LoadTask *load = new LoadTask;
            load->network_ptr = dnn->net_ptr;
            auto descr = layerDescriptions[i];
            load->taskName = dnn->networkName + "-load-" + descr.name;
            load->layerId = descr.layerId;
            load->needsLoading = descr.hasModelFile;
            load->pathToPartial = descr.partialFileName;

            load->id = TASKID_COUNTER++;
            if (dnn->firstTask == nullptr)
            {
                dnn->firstTask = load;
            }
            tasks.push_back(load);


            ExecTask *exec = new ExecTask;

            exec->network_ptr = dnn->net_ptr;
            descr = layerDescriptions[i];
            exec->taskName = dnn->networkName + "-exec-" + descr.name;
            exec->layerId = descr.layerId;
            exec->id = TASKID_COUNTER++;
            if (lastTask != nullptr)
            {
                exec->addTaskDependency(lastTask);
            }
            exec->addTaskDependency(load);
            lastTask = exec;
            tasks.push_back(exec);
        }
        dnn->lastTask = lastTask;
    }

    const std::vector<Task *> &InferenceNetwork::getTasks() const
    {
        return tasks;
    }

    void InferenceNetwork::createTasksLinear()
    {
        InferenceSubTask *dnn = subTasks.front();
        int numLayers = dnn->net_ptr->layers().size();
        Task *lastTask = nullptr;

        for (int i = 0; i < numLayers; ++i)
        {
            LoadTask *load = new LoadTask;
            load->network_ptr = dnn->net_ptr;
            auto descr = layerDescriptions[i];
            load->taskName = dnn->networkName + "-load-" + descr.name;
            load->layerId = descr.layerId;
            load->needsLoading = descr.hasModelFile;
            load->pathToPartial = descr.partialFileName;

            load->id = TASKID_COUNTER++;

            if (lastTask != nullptr)
            {
                load->addTaskDependency(lastTask);
            }
            lastTask = load;

            tasks.push_back(load);
            if (dnn->firstTask == nullptr)
            {
                dnn->firstTask = load;
            }


            ExecTask *exec = new ExecTask;

            exec->network_ptr = dnn->net_ptr;
            descr = layerDescriptions[i];
            exec->taskName = dnn->networkName + "-exec-" + descr.name;
            exec->layerId = descr.layerId;
            exec->id = TASKID_COUNTER++;


            if (lastTask != nullptr)
            {
                exec->addTaskDependency(lastTask);
            }
            lastTask = exec;
            tasks.push_back(exec);
        }
        dnn->lastTask = lastTask;

    }

    void InferenceNetwork::createTasksConvFC()
    {

        InferenceSubTask *dnn = subTasks.front();
        int numLayers = dnn->net_ptr->layers().size();

        int numConv = dnn->num_conv;
        int numFC = dnn->num_fc;
        Task *lastTask = nullptr;

        bool hasInputLayer = dnn->hasInputLayer;

        int poolId = 0;
        for (int i = 0; i < numLayers; ++i)
        {
            if (i < numConv)
            {
                poolId = 0;
            } else
            {
                poolId = 1;
            }

            LoadTask *load = new LoadTask;

            load->network_ptr = dnn->net_ptr;
            auto descr = layerDescriptions[i];
            load->taskName = dnn->networkName + "-load-" + descr.name;
            load->layerId = descr.layerId;
            load->needsLoading = descr.hasModelFile;
            load->pathToPartial = descr.partialFileName;

            load->id = TASKID_COUNTER++;

            load->assignedPoolId = poolId;

            tasks.push_back(load);
            if (dnn->firstTask == nullptr)
            {
                dnn->firstTask = load;
            }

            ExecTask *exec = new ExecTask;

            exec->network_ptr = dnn->net_ptr;
            descr = layerDescriptions[i];
            exec->taskName = dnn->networkName + "-exec-" + descr.name;
            exec->layerId = descr.layerId;
            exec->id = TASKID_COUNTER++;
            if (lastTask != nullptr)
            {
                exec->addTaskDependency(lastTask);
            }
            exec->addTaskDependency(load);
            exec->assignedPoolId = poolId;
            lastTask = exec;
            tasks.push_back(exec);
        }
        dnn->lastTask = lastTask;
    }

    void InferenceNetwork::createTasksBulk()
    {
        InferenceSubTask *dnn = subTasks.front();
        int numLayers = dnn->net_ptr->layers().size();
        Task *lastLoadTask = nullptr;
        Task *lastExecTask = nullptr;
        Task *firstExecTask = nullptr;

        bool hasInputLayer = dnn->hasInputLayer;

        for (int i = 0; i < numLayers; ++i)
        {
            LoadTask *load = new LoadTask;

            load->network_ptr = dnn->net_ptr;
            auto descr = layerDescriptions[i];
            load->taskName = dnn->networkName + "-load-" + descr.name;
            load->layerId = descr.layerId;
            load->needsLoading = descr.hasModelFile;
            load->pathToPartial = descr.partialFileName;
            load->id = TASKID_COUNTER++;

            if (lastLoadTask == nullptr)
            {
                load->addTaskDependency(lastLoadTask);
            }
            lastLoadTask = load;

            tasks.push_back(load);
            if (dnn->firstTask != nullptr)
            {
                dnn->firstTask = load;
            }


            ExecTask *exec = new ExecTask;
            exec->network_ptr = dnn->net_ptr;
            descr = layerDescriptions[i];
            exec->taskName = dnn->networkName + "-exec-" + descr.name;
            exec->layerId = descr.layerId;
            exec->id = TASKID_COUNTER++;

            if (firstExecTask == nullptr)
            {
                firstExecTask = exec;
            }

            if (lastExecTask != nullptr)
            {
                exec->addTaskDependency(lastExecTask);
            }
            lastExecTask = exec;
            tasks.push_back(exec);
        }
        if (firstExecTask != nullptr)
            firstExecTask->addTaskDependency(lastLoadTask);
        dnn->lastTask = lastExecTask;
    }

    bool InferenceNetwork::isFinished()
    {
        bool finished = true;

        for (InferenceSubTask *subTask : subTasks)
        {
            if (subTask->lastTask && !subTask->lastTask->executed)
            {
                finished = false;
            }
        }
        return finished;
    }


    void InferenceNetwork::createTasks(int splittingPolicy)
    {
        std::string policyName = "";
        if (splittingPolicy == 0)
        {
            policyName = "Bulk";
        } else if (splittingPolicy == 1)
        {
            policyName = "DeepEye";
        } else if (splittingPolicy == 3)
        {
            policyName = "Linear";
        } else
        {
            policyName = "Partial";
        }

        if (splittingPolicy == 0) // bulk
        {
            createTasksBulk();
        } else if (splittingPolicy == 1)
        { // conv-fc | deepeye
            createTasksConvFC();
        } else if (splittingPolicy == 3)
        { // Linear
            createTasksLinear();
        } else
        { // Partial
            createTasks();
        }

    }

    void InferenceNetwork::showResult()
    {
        InferenceSubTask *ptr = subTasks.front();

        if (ptr->resultVector.size() == 0)
        {
            std::cout << "No resultvector to show results from " << std::endl;
            return;
        }

        caffe::Blob<float> *output_layer = ptr->net_ptr->output_blobs()[0];

        const float *begin = output_layer->cpu_data();
        const float *end = begin + output_layer->channels();
        std::vector<float> result = std::vector<float>(begin, end);

        auto it = max_element(std::begin(result), std::end(result));
        long pos = it - std::begin(result);

        std::string output = ptr->resultVector[pos];

        std::cout << "The output of " << ptr->networkName << " is: '" << output << "'" << std::endl;

        cv::imshow("Output: " + output, ptr->origInputData);
        cv::waitKey();

    }

    /**
     * Deallocator
     * Clean up the linked references
     */
    InferenceNetwork::~InferenceNetwork()
    {
        // Delete all the subtasks
        for (auto subtask : subTasks)
            if (subtask != nullptr)
                delete subtask;

        // Delete tasks is needed
        for (auto task : tasks)
            if (task != nullptr)
                delete task;
    }
}