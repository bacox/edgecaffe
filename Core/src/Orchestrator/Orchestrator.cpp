//
// Created by bacox on 27/07/2020.
//


#include <Orchestrator/Orchestrator.h>
#include <Util/Config.h>
#include <GeneratedNetwork.h>

EdgeCaffe::Orchestrator::Orchestrator()
{
    // Get config
    Config &config = Config::getInstance();
    verbose = config.verbose();
    mode = config.modeAsType();
    arrivalMode = config.arrivalModeAsType();
    modeAsString = config.mode();
    numberOfWorkers = config.numberOfWorkers();
    mc->interNetworkCondition = &enforceInterDependencies;
    mc->setCapacity(config.memoryLimitAsNum());
    // Lock the 'memory footprint' of EdgeCaffe
    mc->lockMemory(4.87);

//    setup();


}

void EdgeCaffe::Orchestrator::setArrivals(EdgeCaffe::ArrivalList arrivals)
{
    Orchestrator::arrivals = arrivals;
}

void EdgeCaffe::Orchestrator::start()
{
    #ifdef MEMORY_CHECK_ON
    // This will only be used when the MEMORY_CHECK_ON is set in CMAKE
        perf.startTracking();
        perf.start();
    #endif


    for( const auto& worker : workers){
        worker->verbose = verbose;
        worker->run();
    }
    previousTimePoint = std::chrono::high_resolution_clock::now();
}

void EdgeCaffe::Orchestrator::processTasks()
{
    while (!allowedToStop())
    {
        checkArrivals();
        checkBagOfTasks();
        checkFinishedNetworks();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void EdgeCaffe::Orchestrator::waitForStop()
{
    for(const auto worker : workers)
    {
        worker->allowed_to_stop = true;
    }
    std::cout << "Waiting for workers to stop" << std::endl;
    for (const auto worker : workers)
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

void EdgeCaffe::Orchestrator::checkArrivals()
{
    if(arrivalMode == Type::ARRIVAL_MODE::BATCH)
    {
        // Only check if there are no networks active, hence the batch is finished --> start a new batch
        if(nr->numActiveNetworks() > 0)
            return;
    }

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

void EdgeCaffe::Orchestrator::checkFinishedNetworks()
{
    // Check if networks are done and can be deallocated fully
    for (auto inferenceTask : inferenceTasks)
    {
        if (!inferenceTask->finished && inferenceTask->net->isFinished())
        {
            // Create results obj
            inferenceTask->finished = true;

            // deallocate
            inferenceTask->output.policy = modeAsString;
            auto netId = inferenceTask->net->networkId;
            inferenceTask->dealloc();
            nr->deactivateNetwork();
//                std::cout << "DEALLOC network id: " << netId << std::endl;



        }
    }

    if(allFinished())
        last = nullptr;
}

bool EdgeCaffe::Orchestrator::allowedToStop()
{
    return arrivals.isEmpty() && bagOfTasks.size() <= 0 && allFinished();
}

bool EdgeCaffe::Orchestrator::allFinished()
{
    bool finished = true;
    for (auto inferenceTask : inferenceTasks)
    {
        if (!inferenceTask->finished)
            finished = false;
    }
    return finished;
}

void EdgeCaffe::Orchestrator::submitInferenceTask(const EdgeCaffe::Arrival arrivalTask, bool use_scales)
{
    for(const auto& arrivalNetwork : arrivalTask.networks)
    {
        InferenceTask *iTask = new InferenceTask;
//            iTask->pathToNetwork = arrivalTask.pathToNetwork;
        auto &globalConfig = Config::getInstance();
        iTask->pathToNetwork = arrivalNetwork.pathToNetwork;
        iTask->pathToData = globalConfig.pathToResources() + "/" + arrivalTask.pathToData;
        // Load yaml
//            std::string pathToDescription = arrivalTask.pathToNetwork;
        std::string pathToDescription = globalConfig.pathToNetworks() + "/" + arrivalNetwork.pathToNetwork;
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
            iTask->net->dataPath = globalConfig.pathToResources() + "/" + arrivalTask.pathToData;
            iTask->output.networkName = iTask->net->subTasks.front()->networkName;
        }
        iTask->net->meanExecutionTime = description["mean-execution-time"].as<double>(std::numeric_limits<double>::max());
        iTask->net->mc = this->mc.get();

        for(auto tp : taskPools)
            iTask->net->taskpools.push_back(tp);
        iTask->net->bagOfTasks_ptr = &bagOfTasks;
        iTask->net->networkId = EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER;
        EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER++;
        iTask->net->networkProfile.measure(NetworkProfile::ARRIVAL);
        inferenceTasks.push_back(iTask);
        iTask->net->createTasks(mode);
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

        if(mode == Type::MODE_TYPE::LINEAR || mode == Type::MODE_TYPE::BULK || mode == Type::MODE_TYPE::EXECPRIO_INTER)
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

void EdgeCaffe::Orchestrator::processResults()
{

}

void EdgeCaffe::Orchestrator::processLayerData(const std::string &pathToFile)
{
    std::vector<std::string> layerOutputLines;
    for (auto inferenceTasks : inferenceTasks)
        for (std::string line : inferenceTasks->output.toCsvLines())
            layerOutputLines.push_back(line);
    output.toCSV(pathToFile, layerOutputLines, EdgeCaffe::Output::LAYER);
}

void EdgeCaffe::Orchestrator::processEventData(
        const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start
)
{
    std::vector<EdgeCaffe::InferenceOutput::event> taskEvents;
    for (auto inferenceTasks : inferenceTasks)
    {
        auto outputObj = inferenceTasks->output;
        outputObj.getTaskEvents(taskEvents, start);
    }
    auto lines_tasks = EdgeCaffe::InferenceOutput::calculateTaskProfile(taskEvents);
    output.toCSV(pathToFile, lines_tasks, EdgeCaffe::Output::QUEUE);
}

void EdgeCaffe::Orchestrator::processNetworkData(
        const std::string &pathToFile, std::chrono::time_point<std::chrono::system_clock> start
)
{
    std::vector<std::string> networkLines;
    int networkId = 0;
    for (auto inferenceTasks : inferenceTasks)
    {
        std::string networkName = inferenceTasks->pathToNetwork;
        std::string line = inferenceTasks->output.netProfile.durationAsCSVLine(networkId, networkName, start);
        networkLines.push_back(line);
        networkId++;
    }
    output.toCSV(pathToFile, networkLines, EdgeCaffe::Output::NETWORK);
}

const std::shared_ptr<EdgeCaffe::NetworkRegistry> &EdgeCaffe::Orchestrator::getNr() const
{
    return nr;
}

const std::vector<EdgeCaffe::InferenceTask *> &EdgeCaffe::Orchestrator::getInferenceTasks() const
{
    return inferenceTasks;
}
