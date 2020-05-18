//
// Created by bacox on 08/04/2020.
//

#ifndef EDGECAFFE_ORCHESTRATOR_H
#define EDGECAFFE_ORCHESTRATOR_H


#include <vector>
#include <Tasks/LoadTask.h>
#include <Tasks/ExecTask.h>
#include <Tasks/DummyTask.h>
#include "TaskPool.h"
#include "Worker.h"
#include "InferenceNetwork.h"
#include "InferenceOutput.h"
#include "ArrivalList.h"

namespace EdgeCaffe
{
    /**
     * Stucture to hold the references and the data of the network, tasks and the layers during execution
     */
    struct InferenceTask
    {
        std::string pathToNetwork;
        std::string pathToData;
        InferenceNetwork *net;

        bool finished = false;

        InferenceOutput output;

        void dealloc()
        {
            net->networkProfile.measure(NetworkProfile::STOP);
            output.netProfile = net->networkProfile;
            auto ptr = net->subTasks.front()->net_ptr;
            std::vector<std::string> layerNames;
            if(ptr)
            {
                layerNames = net->subTasks.front()->net_ptr->layer_names();
            } else {
                for(auto t : net->tasks){
                    std::string layerName = "layer-" + std::to_string(t->layerId);
                    layerNames.push_back(layerName);
                }
                auto last = std::unique(layerNames.begin(), layerNames.end());
                layerNames.erase(last, layerNames.end());
            }
            output.initFromLayerVector(layerNames);
//
            for (auto task : net->tasks)
            {
                task->measureTime(Task::TIME::FINISHED);
                if (dynamic_cast<LoadTask *>(task))
                {
//                // Load Task
                    output.setLoadingTime(task);
                    output.addTaskProfile(task, true);
                }
                if (dynamic_cast<ExecTask *>(task))
                {
//                // Load Task
                    output.setExecutionTime(task);
                    output.addTaskProfile(task, false);
                }
                if (auto dt = dynamic_cast<DummyTask *>(task))
                {
                    if(dt->isLoadingTask)
                    {
                        output.setLoadingTime(task);
                        output.addTaskProfile(task, true);
                    }
                    else
                    {
                        output.setExecutionTime(task);
                        output.addTaskProfile(task, false);
                    }
                }
            }

            delete net;
        };
    };


    class Orchestrator
    {
    public:
        Orchestrator();

        enum MODEL_SPLIT_MODE
        {
            BULK = 0,
            DEEPEYE = 1,
            PARTIAL = 2,
            LINEAR = 3
        };

        std::vector<Worker *> workers;
        std::vector<TaskPool *> taskPools;

        TaskPool outPool;

        std::vector<Task *> bagOfTasks;

        std::vector<InferenceTask *> inferenceTasks;

        ArrivalList arrivals;

        Task * last = nullptr;

        bool allFinished();

        // Set mode
        MODEL_SPLIT_MODE splitMode = PARTIAL;
        std::string splitModeAsString;

        void checkBagOfTasks();
        void checkFinishedNetworks();
        void checkArrivals();

        bool allowedToStop();

        std::chrono::time_point<std::chrono::system_clock> previousTimePoint;

        // Setup DeepEye mode
        // Setup Bulk mode
        // Setup Partial loading mode
        void setupBulkMode();

        #ifdef MEMORY_CHECK_ON
        // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        void setupLinearMode(MemCheck *perf);
        #else
        void setupLinearMode();
        #endif

        void setupDeepEyeMode();

        void setupPartialMode(int numberOfWorkers);

        void start();

        void waitForStop();

        void submitInferenceTask(const std::string &networkPath, const std::string &dataPath, bool use_scales = false);
        void submitInferenceTask(const Arrival  arrivalTask, bool use_scales = false);

        void processTasks();

    public:
        virtual ~Orchestrator();
    };
}


#endif //EDGECAFFE_ORCHESTRATOR_H
