//
// Created by bacox on 08/04/2020.
//

#include <opencv2/imgcodecs.hpp>
#include <GeneratedNetwork.h>
#include <PriorityTaskPool.h>
#include "../include/Orchestrator.h"

namespace EdgeCaffe
{
    void Orchestrator::start()
    {
        #ifdef MEMORY_CHECK_ON
        // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        perf.startTracking();
        perf.start();
        #endif

        for (Worker *w : workers)
        {
            w->run();
        }
        previousTimePoint = std::chrono::high_resolution_clock::now();
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

        for(const auto& arrivalNetwork : arrivalTask.networks)
        {
            InferenceTask *iTask = new InferenceTask;
//            iTask->pathToNetwork = arrivalTask.pathToNetwork;
            iTask->pathToNetwork = arrivalNetwork.pathToNetwork;
            iTask->pathToData = arrivalTask.pathToData;
            // Load yaml
//            std::string pathToDescription = arrivalTask.pathToNetwork;
            std::string pathToDescription = arrivalNetwork.pathToNetwork;
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
//                iTask->net = new GeneratedNetwork(arrivalTask.pathToNetwork);
                iTask->net = new GeneratedNetwork(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
                iTask->net->init(description);
                iTask->output.networkName = iTask->net->subTasks.front()->networkName;
            } else {
                // Default network
//                iTask->net = new InferenceNetwork(arrivalTask.pathToNetwork);
                iTask->net = new InferenceNetwork(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
                iTask->net->init(description);
                iTask->net->use_scales = description["use-scales"].as<bool>(false);
                iTask->net->dataPath = arrivalTask.pathToData;
                iTask->output.networkName = iTask->net->subTasks.front()->networkName;
            }
            iTask->net->mc = &this->mc;

            for(auto tp : taskPools)
                iTask->net->taskpools.push_back(tp);
            iTask->net->bagOfTasks_ptr = &bagOfTasks;
            iTask->net->networkId = EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER;
            EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER++;
            iTask->net->networkProfile.measure(NetworkProfile::ARRIVAL);
            inferenceTasks.push_back(iTask);
            iTask->net->createTasks(splitMode);
            std::vector<Task *> listOfTasks = iTask->net->getTasks();

            /**
             * Do not use the conditional constraints for. Just force linearity with hard constraints
             */
//            /**
//             * Lets give all networks optional constraints
//             * The constraints are not enforced by default.
//             * We can optimize to later on by using two lists for conditional constraints and normal constraints
//             */
//            if(last != nullptr)
//            {
//                auto obj = ConditionalDependency(last, &this->enforceInterDependencies);
//                iTask->net->subTasks.front()->firstTask->addTaskDependency(ConditionalDependency(last, &this->enforceInterDependencies));
//            }

            if (splitMode == MODEL_SPLIT_MODE::LINEAR || splitMode == MODEL_SPLIT_MODE::BULK || splitMode == MODEL_SPLIT_MODE::PRIO_EXEC_INTER)
            {
                if(last != nullptr)
                {
                    iTask->net->subTasks.front()->firstTask->addTaskDependency(TaskDependency(last));
                }
            }
            for(auto task : listOfTasks)
                task->measureTime(Task::TIME::TO_WAITING);
//            last = listOfTasks.back();
            last = iTask->net->subTasks.front()->lastTask;


            bagOfTasks.reserve(listOfTasks.size()); // preallocate memory
            bagOfTasks.insert(bagOfTasks.end(), listOfTasks.begin(), listOfTasks.end());

        }
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

        #ifdef MEMORY_CHECK_ON
        // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        perf.stopTracking();

        perf.stop();
        perf.join();
        #endif
    }

    void Orchestrator::processTasks()
    {
        while (!allowedToStop())
        {
            checkArrivals();
            checkBagOfTasks();
            checkFinishedNetworks();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    Orchestrator::Orchestrator()
    {
        inferenceTasks.push_back(nullptr);
        inferenceTasks[0] = nullptr;
        inferenceTasks.clear();
        mc.interNetworkCondition = &enforceInterDependencies;
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

    void Orchestrator::checkBagOfTasks()
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
                task->measureTime(Task::TIME::TO_READY);
            }
        }
    }

    void Orchestrator::checkFinishedNetworks()
    {
        // Check if networks are done and can be deallocated fully
        for (auto inferenceTask : inferenceTasks)
        {
            if (!inferenceTask->finished && inferenceTask->net->isFinished())
            {
                // Create results obj
                inferenceTask->finished = true;

                // deallocate
                inferenceTask->output.policy = splitModeAsString;
                auto netId = inferenceTask->net->networkId;
                inferenceTask->dealloc();
                nr.deactivateNetwork();
//                std::cout << "DEALLOC network id: " << netId << std::endl;



            }
        }

        if(allFinished())
            last = nullptr;
    }

    bool Orchestrator::allowedToStop()
    {
        return arrivals.isEmpty() && bagOfTasks.size() <= 0 && allFinished();
//        bagOfTasks.size() > 0 || !allFinished()
    }

    void Orchestrator::checkArrivals()
    {
        if(arrivals.isEmpty())
            return;
        Arrival head = arrivals.first();

        auto current = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( current - previousTimePoint).count();
//        std::cout << "Arrival time check: " << duration << " >= " << head.time << std::endl;
        if(duration >= head.time)
        {
            // Enough time has passed to this arrival to arrive
            if(verbose)
                std::cout << "New arrival " << head.toString() << std::endl;

            submitInferenceTask(head);

            // Remove arrival from arrival list
            arrivals.pop();

            // Update last time measurement
            previousTimePoint = std::chrono::high_resolution_clock::now();
        }




    }

    void Orchestrator::setup(Orchestrator::MODEL_SPLIT_MODE mode,  std::string modeAsString)
    {
        switch (mode)
        {
            case BULK:
                setupBulkMode();
                break;
            case DEEPEYE:
                setupDeepEyeMode();
                break;
            case LINEAR:
                setupLinearMode();
                break;
            case PRIO_EXEC:
                setupExecPrioMode(4);

                // This is not a clean way, but its quick
                // Make sure this is after the setup call
                // Do not enforce inter dependencies to prevent linear inter-network behaviour
                this->enforceInterDependencies = false;
                break;
            case PRIO_EXEC_INTER:
                setupExecPrioMode(4);

                // This is not a clean way, but its quick
                // Make sure this is after the setup call
                // Enforce inter dependencies to create linear inter-network behaviour
                this->enforceInterDependencies = true;
                break;
            case MASA_P:
                setupMasa_P(4);
                break;
            case MASA_E:
                setupMasa_E(4);
                break;
            default:
//                Partial Mode is default
                setupPartialMode(4);
        }
        splitMode = mode;
        splitModeAsString = modeAsString;
    }

    void Orchestrator::setArrivals(ArrivalList arrivals)
    {
        Orchestrator::arrivals = arrivals;
    }

    const std::vector<InferenceTask *> &Orchestrator::getInferenceTasks() const
    {
        return inferenceTasks;
    }

    void Orchestrator::processResults()
    {

    }

    void Orchestrator::processLayerData(const std::string &pathToFile)
    {
        std::vector<std::string> layerOutputLines;
        for (auto inferenceTasks : getInferenceTasks())
            for (std::string line : inferenceTasks->output.toCsvLines())
                layerOutputLines.push_back(line);
        output.toCSV(pathToFile, layerOutputLines, EdgeCaffe::Output::LAYER);
    }

    void Orchestrator::processEventData(const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start)
    {
        std::vector<EdgeCaffe::InferenceOutput::event> taskEvents;
        for (auto inferenceTasks : getInferenceTasks())
        {
            auto outputObj = inferenceTasks->output;
            outputObj.getTaskEvents(taskEvents, start);
        }
        auto lines_tasks = EdgeCaffe::InferenceOutput::calculateTaskProfile(taskEvents);
        output.toCSV(pathToFile, lines_tasks, EdgeCaffe::Output::QUEUE);
    }

    void Orchestrator::processNetworkData(
            const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start
    )
    {
        std::vector<std::string> networkLines;
        int networkId = 0;
        for (auto inferenceTasks : getInferenceTasks())
        {
            std::string networkName = inferenceTasks->pathToNetwork;
            std::string line = inferenceTasks->output.netProfile.durationAsCSVLine(networkId, networkName, start);
            networkLines.push_back(line);
            networkId++;
        }
        output.toCSV(pathToFile, networkLines, EdgeCaffe::Output::NETWORK);
    }

    const std::vector<Worker *> &Orchestrator::getWorkers() const
    {
        return workers;
    }

    void Orchestrator::setupBulkMode()
    {
        TaskPool *taskPool = new TaskPool;
        taskPool->mc = &mc;
        taskPool->nr = &nr;
        taskPool->masaEnabled = &masaEnabled;
        taskPools.push_back(taskPool);
        int i = 0;
        workers.push_back(new Worker(taskPool, &outPool, i++));

        // Enforce inter dependencies to create linear inter-network behaviour
        this->enforceInterDependencies = false;
    }

    void Orchestrator::setupDeepEyeMode()
    {
        TaskPool *convPool = new TaskPool;
        convPool->mc = &mc;
        convPool->nr = &nr;
        convPool->masaEnabled = &masaEnabled;
        TaskPool *fcPool = new TaskPool;
        fcPool->mc = &mc;
        fcPool->nr = &nr;
        fcPool->masaEnabled = &masaEnabled;
        taskPools.push_back(convPool);
        taskPools.push_back(fcPool);
        int i = 0;
        workers.push_back(new Worker(convPool, &outPool, i++));
        workers.push_back(new Worker(fcPool, &outPool, i++));

        // Enforce inter dependencies to create linear inter-network behaviour
        this->enforceInterDependencies = true;
    }

    void Orchestrator::setupPartialMode(int numberOfWorkers)
    {

        TaskPool *taskPool = new TaskPool;
        taskPool->mc = &mc;
        taskPool->nr = &nr;
        taskPool->masaEnabled = &masaEnabled;
        taskPools.push_back(taskPool);

        for (int i = 0; i < numberOfWorkers; ++i)
        {
            workers.push_back(new Worker(taskPool, &outPool, i));
        }

        // Do not enforce inter dependencies to prevent linear inter-network behaviour
        this->enforceInterDependencies = false;
    }

    void Orchestrator::setupLinearMode()
    {
        TaskPool *taskPool = new TaskPool;
        taskPool->mc = &mc;
        taskPool->nr = &nr;
        taskPool->masaEnabled = &masaEnabled;
        taskPools.push_back(taskPool);

        int i = 0;
        workers.push_back(new Worker(taskPool, &outPool, i++));

        // Enforce inter dependencies to create linear inter-network behaviour
        this->enforceInterDependencies = true;
        #ifdef MEMORY_CHECK_ON
                // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
                workers.back()->perf = &perf;
        #endif
    }

    void Orchestrator::setupExecPrioMode(int numberOfWorkers)
    {
        TaskPool *taskPool = new PriorityTaskPool;
        taskPool->mc = &mc;
        taskPool->nr = &nr;
        taskPool->masaEnabled = &masaEnabled;
        taskPools.push_back(taskPool);

        for (int i = 0; i < numberOfWorkers; ++i)
        {
            workers.push_back(new Worker(taskPool, &outPool, i));
        }
    }

    void Orchestrator::setupMasa_P(int numberOfWorkers)
    {
        setupPartialMode(numberOfWorkers);
        masaEnabled = true;
    }

    void Orchestrator::setupMasa_E(int numberOfWorkers)
    {
        setupExecPrioMode(numberOfWorkers);
        masaEnabled = true;
    }

    bool Orchestrator::isMasaEnabled() const
    {
        return masaEnabled;
    }

    void Orchestrator::setMasaEnabled(bool masaEnabled)
    {
        Orchestrator::masaEnabled = masaEnabled;
    }
}