//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_ORCHESTRATOR_H
#define EDGECAFFE_ORCHESTRATOR_H
#include <vector>
#include <Tasks/LoadTask.h>
#include <Tasks/ExecTask.h>
#include <Tasks/DummyTask.h>
#include <Util/Output.h>
#include <Worker.h>
#include <MemoryCounter.h>
#include <NetworkRegistry.h>
#include <InferenceTask.h>
#include <ArrivalList.h>
#include <Scheduler/Scheduler.h>
#include <TaskPool/AbstractTaskPool.h>
#include <InferenceNetwork.h>
#include <InferenceOutput.h>

#ifdef MEMORY_CHECK_ON
// This will only be used when the MEMORY_CHECK_ON is set in CMAKE
#include <Profiler/MemCheck.h>
#endif
namespace EdgeCaffe {

    /**
     * Stucture to hold the references and the data of the network, tasks and the layers during execution
     */
    class Orchestrator
    {
    protected:
    public:
        const std::shared_ptr<NetworkRegistry> &getNr() const;

    protected:
        std::vector<std::shared_ptr<Worker>> workers;
        std::vector<std::shared_ptr<Scheduler>> schedulers;
        std::shared_ptr<NetworkRegistry> nr = std::make_shared<NetworkRegistry>();
        std::shared_ptr<MemoryCounter> mc = std::make_shared<MemoryCounter>();

        Type::MODE_TYPE mode = Type::MODE_TYPE::BULK;
        std::string modeAsString;
        Output output;
    public:
        const std::vector<std::shared_ptr<Worker>> &getWorkers() const
        {
            return workers;
        }

    protected:
        int numberOfWorkers = 1;

    protected:
        bool verbose = true;
        bool enforceInterDependencies = false;
        std::vector<std::shared_ptr<AbstractTaskPool>> taskPools;
        std::shared_ptr<TaskPool> outPool = std::make_shared<TaskPool>();
        std::vector<Task *> bagOfTasks;
        std::vector<InferenceTask *> inferenceTasks;
        ArrivalList arrivals;
        Task * last = nullptr;

        std::chrono::time_point<std::chrono::system_clock> previousTimePoint;
#ifdef MEMORY_CHECK_ON
        // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        MemCheck perf;
#endif
        virtual void setup() = 0;

        bool allowedToStop();
        virtual void checkBagOfTasks()= 0;
        void checkFinishedNetworks();
        void checkArrivals();
        bool allFinished();

    public:
        void start();
        void processTasks();
        void waitForStop();
        void submitInferenceTask(const Arrival  arrivalTask, bool use_scales = false);
        void setArrivals(ArrivalList arrivals);

        Orchestrator();

        /* Data export functions */
        void processResults();

        const std::vector<InferenceTask *> &getInferenceTasks() const;

        void processLayerData(const std::string &pathToFile);
        void processEventData(const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start);
        void processNetworkData(const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start);

    };
}

#endif //EDGECAFFE_ORCHESTRATOR_H
