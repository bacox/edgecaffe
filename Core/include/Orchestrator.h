//
// Created by bacox on 08/04/2020.
//

#ifndef EDGECAFFE_ORCHESTRATOR_H
#define EDGECAFFE_ORCHESTRATOR_H


#include <vector>
#include <Tasks/LoadTask.h>
#include <Tasks/ExecTask.h>
#include "TaskPool.h"
#include "Worker.h"
#include "InferenceNetwork.h"
#include "InferenceOutput.h"

struct InferenceTask{
    std::string pathToNetwork;
    std::string pathToData;
    InferenceNetwork *net;
    cv::Mat input_img;

    bool finished = false;

    InferenceOutput output;

    void dealloc() {
        std::vector<std::string> layerNames = net->subTasks.front()->net_ptr->layer_names();
        output.initFromLayerVector(layerNames);
//
        for(auto task : net->tasks)
        {
//            if (dynamic_cast<LoadTask *>(task))
//            {
//                // Load Task
                output.setLoadingTime(task);
//            }
//            if (dynamic_cast<ExecTask *>(task))
//            {
////                // Load Task
////                output.setExecutionTime(task);
//            }

        }


        delete net;
    };
};


class Orchestrator {
public:
    Orchestrator();

    enum MODEL_SPLIT_MODE {
        BULK = 0,
        DEEPEYE = 1,
        PARTIAL = 2,
        LINEAR = 3
    };

    std::vector<Worker*> workers;
    std::vector<TaskPool*> taskPools;
    std::vector<InferenceNetwork*> networks;

    TaskPool outPool;

    std::vector<Task*> bagOfTasks;

    std::vector<InferenceTask *> inferenceTasks;

    bool allFinished();

    // Set mode
    MODEL_SPLIT_MODE splitMode = PARTIAL;
    std::string splitModeAsString;

    // Setup DeepEye mode
    // Setup Bulk mode
    // Setup Partial loading mode
    void setupBulkMode();
    void setupLinearMode();
    void setupDeepEyeMode();
    void setupPartialMode(int numberOfWorkers);

    void start();

    void waitForStop();

    void submitInferenceTask(const std::string& networkPath, const std::string& dataPath, bool use_scales = false);

    void processTasks();

public:
    virtual ~Orchestrator();
};


#endif //EDGECAFFE_ORCHESTRATOR_H
