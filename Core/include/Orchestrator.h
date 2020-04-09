//
// Created by bacox on 08/04/2020.
//

#ifndef EDGECAFFE_ORCHESTRATOR_H
#define EDGECAFFE_ORCHESTRATOR_H


#include <vector>
#include "TaskPool.h"
#include "Worker.h"
#include "InferenceNetwork.h"

struct InferenceTask{
    std::string pathToNetwork;
    std::string pathToData;
    InferenceNetwork *net;
    cv::Mat input_img;

    bool finished = false;

    void dealloc() {
        delete net;
    };
};


class Orchestrator {
public:
    Orchestrator();

    enum MODEL_SPLIT_MODE {
        BULK = 0,
        DEEPEYE = 1,
        PARTIAL = 2
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

    // Setup DeepEye mode
    // Setup Bulk mode
    // Setup Partial loading mode
    void setupBulkMode();
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
