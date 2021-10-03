//
// Created by bacox on 30/03/2020.
//

#include "../include/InferenceNetwork.h"
#include "../include/Tasks/LoadTask.h"
#include "../include/Tasks/ExecTask.h"
#include <yaml-cpp/yaml.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <BaseNet.h>
#include <caffe/caffe.hpp>
#include <Tasks/InitNetworkTask.h>
#include <algorithm>
#include <string>
#include <Util/Config.h>
#include <Tasks/ExecBulkTask.h>

namespace EdgeCaffe
{
    InferenceNetwork::InferenceNetwork(const std::string &pathToDescription, bool *dependencyCondition) : pathToDescription(pathToDescription), dependencyCondition(dependencyCondition)
    {}

    int InferenceNetwork::TASKID_COUNTER = 0;
    int InferenceNetwork::NETWORKID_COUNTER = 0;

    void InferenceNetwork::init(YAML::Node &description)
    {
        InferenceSubTask *sub = new InferenceSubTask;
        auto &globalConfig = Config::getInstance();
        sub->networkName = description["name"].as<std::string>();

        std::string networkFile = description["network-file"].as<std::string>();
        std::string partialsLocation = description["partials-location"].as<std::string>();
        sub->basePath = description["base-path"].as<std::string>();
        auto batchSize = globalConfig.batchSize();
        sub->modelFileName = "b" + std::to_string(batchSize) + + "_" + networkFile;
        sub->pathToModelFile = globalConfig.pathToNetworks() + "/" + pathToDescription + "/" + networkFile;
        sub->pathToPartials = globalConfig.pathToNetworks() + "/" + pathToDescription + "/" + partialsLocation;

        sub->pathToParamFile = globalConfig.pathToNetworks() + "/" + pathToDescription + "/" + description["model-file"].as<std::string>();

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



        for(const auto& l_layer : description["conv-layers"])
        {
            layerDescriptions.push_back(LayerDescription::FromYaml(l_layer, sub->pathToPartials));
        }

        for(const auto& l_layer : description["fc-layers"])
        {
            layerDescriptions.push_back(LayerDescription::FromYaml(l_layer, sub->pathToPartials));
        }

        for( const auto& l_layer : layerDescriptions)
        {
            sub->partialNames.push_back(sub->pathToPartials + "/" + l_layer.partialFileName);
        }
        std::string memoryKey = globalConfig.memoryKey();
        maxMemoryUsage = description[memoryKey].as<double>(0);

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

      // Increase estimated memory by batchsize
      for(auto subtask : subTasks){
        for(const auto& task : subtask->tasks) {
          task->requiredMemory *= (double) globalConfig.batchSize();
        }
      }

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

    void InferenceNetwork::createPartialTasks()
    {
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();
        std::shared_ptr<Task> lastTask = nullptr;

        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;
        lastTask = init;

        /**
         * Linking information:
         * Mode: Partial
         * All loading tasks depend on the init task
         * All execution tasks depend on its realated loading task
         */

        for (int i = 0; i < numLayers; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            // Make sure that all load tasks are dependend on the init task
            load->addTaskDependency(TaskDependency(init));
//            bool * valid;
//            load->addTaskDependency(ConditionalDependency(nullptr, init, valid));
            tasks.push_back(load);

            auto exec = createExecTask(dnn, layerDescription);
            // Set a dependency for the previous exectask
            if (lastTask != nullptr)
            {
                exec->addTaskDependency(TaskDependency(lastTask));
            }
            // Set the dependency for this execution task to the last loading task
            exec->addTaskDependency(TaskDependency(load));
            // Update the last task to this task
            lastTask = exec;
            tasks.push_back(exec);
        }
        // Update the last task of the network for intra-network linking
        dnn->lastTask = lastTask;
    }

    const std::vector<std::shared_ptr<Task>> &InferenceNetwork::getTasks() const
    {
        return tasks;
    }

    void InferenceNetwork::createTasksLinear()
    {
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();
        std::shared_ptr<Task> lastTask = nullptr;

        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;
        lastTask = init;

        /**
         * Linking information:
         * Mode: Linear
         * The loading tasks depend on the previous task (exec or init task)
         * The execution tasks depend on the previous task (loading task)
         */
        for (int i = 0; i < numLayers; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            // Make sure the loading task is dependent on the last task
            load->addTaskDependency(TaskDependency(lastTask));
            // Update the last task
            lastTask = load;
            tasks.push_back(load);

            auto exec = createExecTask(dnn, layerDescription);
            // Make sure the exec task is dependent on the last task
            exec->addTaskDependency(TaskDependency(lastTask));
            lastTask = exec;
            tasks.push_back(exec);
        }
        // Update the last task of the network for intra-network linking
        dnn->lastTask = lastTask;
    }

    void InferenceNetwork::createTasksConvFC()
    {
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();
        std::shared_ptr<Task> lastTask = nullptr;

        // Keep track of the taskPool Id; set to pool 0 for now
        int poolId = 0;

        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);
        init->assignedPoolId = poolId;
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;
        lastTask = init;

        /**
         * Linking information:
         * Mode: DeepEye
         * Needs to make the split between the conv-layers and the fc-layers
         */

        int numConv = dnn->num_conv;
        int numFC = dnn->num_fc;

        // Need to keep track of some pointers for dependencies
        std::shared_ptr<Task> firstExecFc = nullptr;
        std::shared_ptr<Task> lastExecConv = nullptr;
        std::shared_ptr<Task> lastExecFc = nullptr;
        std::shared_ptr<Task> firstLoadFc = nullptr;
        std::shared_ptr<Task> firstLoadConv = nullptr;
        std::shared_ptr<Task> lastLoadFc = nullptr;
        std::shared_ptr<Task> lastLoadConv = nullptr;

        // Loop for the conv layers
        for(int i = 0; i < numConv; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            load->assignedPoolId = poolId;
            if(firstLoadConv == nullptr)
            {
                // First loading task of the conv layers
                // This is dependent on the init task
                bool *valid;
                load->addTaskDependency(TaskDependency(init));
                firstLoadConv = load;
            } else
            {
                load->addTaskDependency(TaskDependency(lastTask));
            }
            lastLoadConv = load;
            tasks.push_back(load);
            lastTask = load;

            auto exec = createExecTask(dnn, layerDescription);
            exec->assignedPoolId = poolId;
            exec->addTaskDependency(TaskDependency(lastTask));
            lastExecConv = exec;
            lastTask = exec;
            tasks.push_back(exec);
        }
        poolId = 1;
        // Loop over the fc layers
        for (int i = numConv; i < numLayers; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            load->assignedPoolId = poolId;
            if(firstLoadFc == nullptr)
            {
                // First loading task of the conv layers
                // This is dependent on the init task
                load->addTaskDependency(TaskDependency(init));
                firstLoadFc = load;
            } else
            {
                load->addTaskDependency(TaskDependency(lastLoadFc));
            }
            lastLoadFc = load;
            tasks.push_back(load);
            lastTask = load;

            auto exec = createExecTask(dnn, layerDescription);
            exec->assignedPoolId = poolId;
            exec->addTaskDependency(TaskDependency(lastTask));
            if(firstExecFc == nullptr)
            {
                // This is the first exec layer of the fc-layers
                // Needs to depend in the last exec-layer of conv layers
                exec->addTaskDependency(TaskDependency(lastExecConv));
                firstExecFc = exec;
            } else
            {
                exec->addTaskDependency(TaskDependency(lastExecFc));
            }
            lastExecFc = exec;
            lastTask = exec;
            tasks.push_back(exec);
        }
        dnn->lastTask = lastTask;
    }

    void InferenceNetwork::createTasksConvFCV2()
    {
        /**
         * Create the tasks according to the DeepEye paper
         */
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();
        std::shared_ptr<Task> lastTask = nullptr;

        // Keep track of the taskPool Id; set to pool 0 for now
        int poolId = 0;
        int numConv = dnn->num_conv;
        int numFC = dnn->num_fc;
        std::shared_ptr<Task> lastLoad = nullptr;
        std::shared_ptr<Task> lastExec = nullptr;

        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);
        init->assignedPoolId = poolId;
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;
        dnn->conv_load_first = init;
        lastTask = init;

        /**
         * Paper extract:
         * "In the convolution-execution thread, the convolution filter parameters of all models are loaded into
         * the memory and the convolution operations begin on the pre-processed input data for each model"
         *
         * This means that we first load all the convolutional layers of all models and then execute them one model at
         * a time.
         */

        // Loop over all the conv layers
        for(int i = 0; i < numConv; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            load->assignedPoolId = poolId;
            if(i == 0)
                load->addTaskDependency(TaskDependency(init));
            else
                load->addTaskDependency(TaskDependency(lastLoad));
            lastLoad = load;
            tasks.push_back(load);
            lastTask = load;
            dnn->conv_load_last = load;


            auto exec = createExecTask(dnn, layerDescription);
            exec->assignedPoolId = poolId;
            if(i == 0)
                dnn->conv_exec_first = exec;
            else
                exec->addTaskDependency(TaskDependency(lastExec));
            dnn->conv_exec_last = exec;
            lastExec = exec;
            lastTask = exec;
            tasks.push_back(exec);
        }

        /**
         * Paper extract:
         * "The data-loading thread which is spawned in parallel with the convolution thread is responsible for
         * loading the FC layer parameters for all models into the mem- ory, again in a pipelined manner
         * (i.e., one model after the other). The objective of the convolution-execution thread is to perform
         * all convolutions on the input image, and pass the results of the final convolution layer of
         * each model to the data-loading thread. When the data-loading thread finishes loading the FC layer
         * parameters for a model, it can use the pre-computed convolution outputs from the
         * convolution-execution and proceed to obtain the final classification results."
         *
         * This means we continue to load the fc layers of all networks.
         * When the fc layers of a network is loaded and the conv layers of that same network are executed, we can
         * start with the execution of the fc layers. If the conv layers are not ready yet, continue to preload
         * the other models.
         */

        poolId = 1;
        // Loop over all the fc layers
        for (int i = numConv; i < numLayers; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);
            load->assignedPoolId = poolId;
            if(i == numConv)
            {
                load->addTaskDependency(TaskDependency(init));
                dnn->fc_load_first = load;
            }
            else
                load->addTaskDependency(TaskDependency(lastLoad));
            lastLoad = load;
            dnn->fc_load_last = load;
            tasks.push_back(load);
            lastTask = load;

            auto exec = createExecTask(dnn, layerDescription);
            exec->assignedPoolId = poolId;
            exec->addTaskDependency(TaskDependency(lastTask));
            if(i == numConv)
            {
                exec->addTaskDependency(TaskDependency(lastLoad));
                exec->addTaskDependency(TaskDependency(dnn->conv_exec_last));
                dnn->fc_exec_first = exec;
            }
            else
                exec->addTaskDependency(TaskDependency(lastExec));
            lastExec = exec;
            dnn->fc_exec_last = exec;
            lastTask = exec;
            tasks.push_back(exec);
        }
        // Edge case for TinyYolo where there do not exist any fully connected layers
        if(numFC > 0)
            dnn->fc_exec_first->addTaskDependency(TaskDependency(dnn->fc_load_last));
        dnn->lastTask = lastTask;
    }

    void InferenceNetwork::createTasksBulk()
    {
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();
        std::shared_ptr<Task> lastTask = nullptr;

        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);

        if(dnn->lastTask != nullptr)
            init->addTaskDependency(TaskDependency(dnn->lastTask));
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;

        /**
         * Linking information:
         * Mode: Bulk
         * All loading tasks depend on the previous loading task except the first
         * All execution tasks depend on the previous execTask except the first
         */

        std::shared_ptr<Task> firstExecTask = nullptr;
        std::shared_ptr<Task> firstLoadingTask = nullptr;
        std::shared_ptr<Task> lastLoadingTask = nullptr;
        std::shared_ptr<Task> lastExecTask = nullptr;

        bool hasInputLayer = dnn->hasInputLayer;

        for (int i = 0; i < numLayers; ++i)
        {
            auto layerDescription = layerDescriptions[i];
            auto load = createLoadTask(dnn, layerDescription);

            // If this is the first loading task make it depend on init
            if(firstLoadingTask == nullptr)
            {
                // Dependency on init
                load->addTaskDependency(TaskDependency(init));
                firstLoadingTask = load;
            } else {
                // Dependency on previous loading task
                load->addTaskDependency(TaskDependency(lastLoadingTask));
            }
            // Update the last loading task
            lastLoadingTask = load;
            // Update the last task in general
            lastTask = load;
            tasks.push_back(load);

            auto exec = createExecTask(dnn, layerDescription);
            if(firstExecTask == nullptr)
            {
                // This is the first exec task
                // Needs to depend on the last loading task; will be set later
                firstExecTask = exec;
            } else {
                // Not the first exec task
                // Needs to depend on the previous exec task
                exec->addTaskDependency(TaskDependency(lastExecTask));
            }
            lastExecTask = exec;
            lastTask = exec;
            tasks.push_back(exec);

        }
        // Set the missing dependency between the last loading task and the first execution task
        if (firstExecTask != nullptr)
            firstExecTask->addTaskDependency(TaskDependency(lastLoadingTask));
        // Update the last task of the network for intra-network linking
        dnn->lastTask = lastExecTask;
    }

    void InferenceNetwork::createTasksBulkV2()
    {
        InferenceSubTask *dnn = subTasks.front();
        caffe::NetParameter param;
        caffe::ReadNetParamsFromTextFileOrDie(dnn->pathToModelFile, &param);
        int numLayers = param.layer_size();


        // Create init task
        std::shared_ptr<Task> lastTask = nullptr;
        // Important to first create the network initialisation task
        auto init = createInitTask(dnn);

        if(dnn->lastTask != nullptr)
            init->addTaskDependency(TaskDependency(dnn->lastTask));
        tasks.push_back(init);
        // Set the init task as the first task of this network for intra-network linking
        dnn->firstTask = init;

        // Create bulk load task
        std::string loadDescription = "Bulk loading";
        std::shared_ptr<LoadTask>load = std::dynamic_pointer_cast<LoadTask>(createLoadTask(dnn, layerDescriptions[0]));
        load->addTaskDependency(TaskDependency(init));
        load->pathToPartial = dnn->pathToParamFile;
        load->needsLoading = true;
        tasks.push_back(load);


        auto  execBulk = createExecTask(dnn, layerDescriptions[0], true);
        execBulk->addTaskDependency(TaskDependency(load));
        tasks.push_back(execBulk);
        dnn->lastTask = execBulk;
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

    void InferenceNetwork::createTasks(Type::MODE_TYPE mode)
    {
        switch(mode)
        {
            case Type::MODE_TYPE::DEEPEYE:
//                createTasksConvFC();
                createTasksConvFCV2();
                break;
            case Type::MODE_TYPE::DEEPEYE_FRUGAL:
                createTasksConvFCV2();
                break;
            case Type::MODE_TYPE::LINEAR:
                createTasksLinear();
                break;
            case Type::MODE_TYPE::PARTIAL:
                createPartialTasks();
                break;
            case Type::MODE_TYPE::MASA:
                createPartialTasks();
                break;
            case Type::MODE_TYPE::EXECPRIO:
                createPartialTasks();
                break;
            case Type::MODE_TYPE::EXECPRIO_INTER:
                createPartialTasks();
                break;
            case Type::MODE_TYPE::MULTI_BULK:
                createTasksBulkV2();
                break;
            default: // Default case is bulk
//                createTasksBulk();
                createTasksBulkV2();
                break;

        }
    }
//    void InferenceNetwork::createTasks(int splittingPolicy)
//    {
//        std::string policyName = "";
//        if (splittingPolicy == 0)
//        {
//            policyName = "Bulk";
//        } else if (splittingPolicy == 1)
//        {
//            policyName = "DeepEye";
//        } else if (splittingPolicy == 3)
//        {
//            policyName = "Linear";
//        } else if (splittingPolicy == 4)
//        {
//            policyName = "execprio";
//        }else if (splittingPolicy == 5)
//        {
//            policyName = "execprio-inter";
//        } else
//        {
//            policyName = "Partial";
//        }
//
//        if (splittingPolicy == 0) // bulk
//        {
//            createTasksBulk();
//        } else if (splittingPolicy == 1)
//        { // conv-fc | deepeye
//            createTasksConvFC();
//        } else if (splittingPolicy == 3)
//        { // Linear
//            createTasksLinear();
//        } else if (splittingPolicy == 4)
//        { // Linear
//            createPartialTasks();
//        }else if (splittingPolicy == 5)
//        { // Linear
//            createPartialTasks();
//        } else
//        { // Partial
//            createPartialTasks();
//        }
////        NETWORKID_COUNTER++;
//    }

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
      std::cout << "[DEALLOC] >>> Deallocating network: " << subTasks.front()->networkName << std::endl;
        // Delete all the subtasks
        for (auto subtask : subTasks)
            if (subtask != nullptr)
                delete subtask;

        // Delete tasks is needed
//        for (auto task : tasks)
//        {
//            // Check if task is alive somewhere else
//            bool taskFoundElsewhere = false;
//            for(auto tmp_pool : taskpools)
//                if(tmp_pool->hasTask(task->id))
//                    taskFoundElsewhere = true;
//
//            for(auto tmp_t : (*bagOfTasks_ptr))
//                if(tmp_t->id == task->id)
//                    taskFoundElsewhere = true;
//
//            if(taskFoundElsewhere)
//                std::cerr << "This will be an invalid pointer to free!" << std::endl;
//            if (task != nullptr)
//                delete task;
//
//        }
    }

    std::shared_ptr<Task> InferenceNetwork::createInitTask(InferenceSubTask *dnn)
    {
//        std::shared_ptr<InitNetworkTask> init = new InitNetworkTask(
        std::shared_ptr<InitNetworkTask> init = std::make_shared<InitNetworkTask>(TASKID_COUNTER++,
                                                                                  networkId,
                                                                                  dnn->networkName + "-init-network");
        init->networkExecutionTime = this->meanExecutionTime;
        init->inet = this;
        init->t_type = Task::INIT;
        init->networkName = dnn->networkName;
        init->taskType = "init";
        init->layerName = "net-init";
        init->use_scales = this->use_scales;
        init->pathToInput =  this->dataPath;
        init->dependencyCondition = this->dependencyCondition;
        init->requiredMemory = this->maxMemoryUsage;
        return init;
    }

    std::shared_ptr<Task> InferenceNetwork::createLoadTask(InferenceSubTask *dnn, const LayerDescription &descr)
    {
        std::shared_ptr<LoadTask> load = std::make_shared<LoadTask>(TASKID_COUNTER++,
                                                                    networkId,
                                                                    dnn->networkName + "-exec-" + descr.name);
//        LoadTask *load = new LoadTask(
//                TASKID_COUNTER++,
//                networkId,
//                dnn->networkName + "-exec-" + descr.name
//        );
        load->networkExecutionTime = this->meanExecutionTime;
        load->t_type = Task::LOAD;
        load->network_ptr = &(dnn->net_ptr);
        load->layerId = descr.layerId;
        load->networkName = dnn->networkName;
        load->taskType = "load";
        load->layerName = descr.name;
        load->needsLoading = descr.hasModelFile;
        load->pathToPartial = descr.partialFileName;
        load->dependencyCondition = this->dependencyCondition;
        return load;
    }

    std::shared_ptr<Task> InferenceNetwork::createExecTask(InferenceSubTask *dnn, const LayerDescription &descr, bool bulk)
    {
        std::shared_ptr<ExecTask> exec;
        if(bulk)
        {
          exec = std::make_shared<ExecBulkTask>(TASKID_COUNTER++,
                                            networkId,
                                            dnn->networkName + "-exec-" + descr.name);
        } else {
          exec = std::make_shared<ExecTask>(TASKID_COUNTER++,
                                            networkId,
                                            dnn->networkName + "-exec-" + descr.name);
        }

//        ExecTask *exec = nullptr;
//        if(bulk)
//        {
//            exec = new ExecBulkTask(
//                    TASKID_COUNTER++,
//                    networkId,
//                    dnn->networkName + "-exec-" + descr.name
//            );
//        } else {
//            exec = new ExecTask(
//                    TASKID_COUNTER++,
//                    networkId,
//                    dnn->networkName + "-exec-" + descr.name
//            );
//        }
        exec->networkExecutionTime = this->meanExecutionTime;
        exec->networkName = dnn->networkName;
        exec->t_type = Task::EXEC;
        exec->taskType = "exec";
        exec->layerName = descr.name;
        exec->network_ptr = &(dnn->net_ptr);
        exec->layerId = descr.layerId;
        exec->dependencyCondition = this->dependencyCondition;
        return exec;
    }

    std::string InferenceNetwork::tasksToDotDebug()
    {
        // Create the name of this network
        std::string name = this->subTasks.front()->networkName + "" + std::to_string(this->networkId);
        // Replace any spaces in the name to prevent errors while running the dot command
        std::replace( name.begin(), name.end(), ' ', '_');

        std::stringstream dotContent;
        // Define beginning of the dot file
        dotContent << "digraph " << name << " {" << std::endl;

        // Get all the definitions of the nodes
        std::vector<std::string> allNodes;
        for(auto task : tasks)
        {
            std::string node = std::to_string(task->id) + "[label = \"" + "("+std::to_string(task->networkId)+") " + task->getTaskDescription() + "\"];";
            allNodes.push_back(node);
            for (auto taskDependency : task->dependsOn)
            {
                auto dep = taskDependency.dependency;
                std::string depNode = std::to_string(dep->id) + "[label = \"" + "("+std::to_string(dep->networkId)+") " + dep->getTaskDescription() + "\"];";
                allNodes.push_back(depNode);
            }
        }

        // Create a lit of unique lines
        std::sort(allNodes.begin(), allNodes.end());
        auto last = std::unique(allNodes.begin(), allNodes.end());
        allNodes.erase(last, allNodes.end());

        // Add the node definitions to the content for the dot file
        for(auto line : allNodes)
            dotContent << line << std::endl;

        // Create the edges between the nodes that represent the dependencies
        for(auto task : tasks)
            for(auto taskDependency : task->dependsOn)
            {
                auto dep = taskDependency.dependency;
                dotContent << std::to_string(task->id) << " -> " << std::to_string(dep->id) << std::endl;
            }
        // End of the dotfile
        dotContent << "}" << std::endl;

        return dotContent.str();
    }

    void InferenceNetwork::createTasksExecPrio()
    {

    }

}