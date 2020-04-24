//
// Created by bacox on 08/04/2020.
//

#include <opencv2/imgcodecs.hpp>
#include <GeneratedNetwork.h>
#include "../include/Orchestrator.h"

namespace EdgeCaffe
{


    void Orchestrator::setupBulkMode()
    {
        TaskPool *taskPool = new TaskPool;
        taskPools.push_back(taskPool);
        int i = 0;
        workers.push_back(new Worker(taskPool, &outPool, i++));

    }

    void Orchestrator::setupDeepEyeMode()
    {
        TaskPool *convPool = new TaskPool;
        TaskPool *fcPool = new TaskPool;
        taskPools.push_back(convPool);
        taskPools.push_back(fcPool);
        int i = 0;
        workers.push_back(new Worker(convPool, &outPool, i++));
        workers.push_back(new Worker(fcPool, &outPool, i++));
    }

    void Orchestrator::setupPartialMode(int numberOfWorkers)
    {

        TaskPool *taskPool = new TaskPool;
        taskPools.push_back(taskPool);

        for (int i = 0; i < numberOfWorkers; ++i)
        {
            workers.push_back(new Worker(taskPool, &outPool, i));
        }
    }

    void Orchestrator::setupLinearMode()
    {
        TaskPool *taskPool = new TaskPool;
        taskPools.push_back(taskPool);

        int i = 0;
        workers.push_back(new Worker(taskPool, &outPool, i++));
    }

    void Orchestrator::start()
    {
        for (Worker *w : workers)
        {
            w->run();
        }
    }

    Orchestrator::~Orchestrator()
    {
        // Remove all taskpools
        for (TaskPool *pool: taskPools)
        {
            delete pool;
        }

        // Remove all workers
        for (Worker *worker: workers)
        {
            delete worker;
        }
    }

    void Orchestrator::submitInferenceTask(Arrival arrivalTask, bool use_scales)
    {
        InferenceTask *iTask = new InferenceTask;
        iTask->pathToNetwork = arrivalTask.pathToNetwork;
        iTask->pathToData = arrivalTask.pathToData;
        // Load yaml
        std::string pathToDescription = arrivalTask.pathToNetwork;
        std::string pathToYaml = pathToDescription + "/description.yaml";
        YAML::Node description;
        try{
            description = YAML::LoadFile(pathToYaml);
        } catch(...){
            std::cerr << "Error while attempting to read yaml file!" << std::endl;
            std::cerr << "Yaml file: " << pathToYaml << std::endl;
        }

        if(description["type"].as<std::string>("normal") == "generated")
        {
            // Generated network
            iTask->net = new GeneratedNetwork(arrivalTask.pathToNetwork);
            iTask->net->init(description);
            iTask->output.networkName = iTask->net->subTasks.front()->networkName;
        } else {
            // Default network
            iTask->net = new InferenceNetwork(arrivalTask.pathToNetwork);
            iTask->net->init(description);
            iTask->output.networkName = iTask->net->subTasks.front()->networkName;

            cv::Mat inputData = cv::imread(arrivalTask.pathToData);
            iTask->net->setInput(inputData, use_scales);
            iTask->net->loadNetworkStructure();
            iTask->net->loadInputToNetwork();
        }


        inferenceTasks.push_back(iTask);

        iTask->net->createTasks(splitMode);
        std::vector<Task *> listOfTasks = iTask->net->getTasks();
        bagOfTasks.reserve(listOfTasks.size()); // preallocate memory
        bagOfTasks.insert(bagOfTasks.end(), listOfTasks.begin(), listOfTasks.end());

    }

    void
    Orchestrator::submitInferenceTask(const std::string &networkPath, const std::string &dataPath, bool use_scales)
    {

        InferenceTask *iTask = new InferenceTask;
        iTask->pathToNetwork = networkPath;
        iTask->pathToData = dataPath;
        cv::Mat input_img = cv::imread(dataPath);
        iTask->net = new InferenceNetwork(networkPath);


        inferenceTasks.push_back(iTask);

        iTask->net->init();
        iTask->output.networkName = iTask->net->subTasks.front()->networkName;
        iTask->net->setInput(input_img, use_scales);
        iTask->net->loadNetworkStructure();
        iTask->net->loadInputToNetwork();
        iTask->net->createTasks(splitMode);
        std::vector<Task *> listOfTasks = iTask->net->getTasks();
        bagOfTasks.reserve(listOfTasks.size()); // preallocate memory
        bagOfTasks.insert(bagOfTasks.end(), listOfTasks.begin(), listOfTasks.end());
    }

    void Orchestrator::waitForStop()
    {
        for (Worker *worker: workers)
        {
            worker->allowed_to_stop = true;
        }
        std::cout << "Waiting for workers to stop" << std::endl;
        for (Worker *worker : workers)
        {
            worker->_thread.join();
        }
    }

    void Orchestrator::processTasks()
    {
        while (bagOfTasks.size() > 0 || !allFinished())
        {
            // Check if new tasks are available to insert in the taskpool
            for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
            {
                // remove odd numbers
                Task *task = *it;
                if (!task->waitsForOtherTasks())
                {
                    bagOfTasks.erase(it--);
                    if (task->hasPoolAssigned())
                    {
                        int poolId = task->getAssignedPoolId();
                        taskPools[poolId]->addTask(task);
                    } else
                    {
                        taskPools.front()->addTask(task);
                    }
                }
            }

            // Check if networks are done and can be deallocated fully
            for (auto inferenceTask : inferenceTasks)
            {
                if (!inferenceTask->finished && inferenceTask->net->isFinished())
                {
                    // Create results obj
                    inferenceTask->finished = true;

                    // deallocate
                    inferenceTask->output.policy = splitModeAsString;
                    inferenceTask->dealloc();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    Orchestrator::Orchestrator()
    {
        inferenceTasks.push_back(nullptr);
        inferenceTasks[0] = nullptr;
        inferenceTasks.clear();
    }

    bool Orchestrator::allFinished()
    {
        bool finished = true;
        for (auto inferenceTask : inferenceTasks)
        {
            if (!inferenceTask->finished)
                finished = false;
        }
        return finished;
    }


}