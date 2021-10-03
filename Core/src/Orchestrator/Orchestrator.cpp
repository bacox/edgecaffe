//
// Created by bacox on 27/07/2020.
//


#include <Orchestrator/Orchestrator.h>
#include <Util/Config.h>
#include <GeneratedNetwork.h>
#include <Scheduler/MasaScheduler.h>

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

    overideWithDataLabel = config.overideWithDataLabel.valueOrDefault();
    waitForNetworkFinish = config.waitForNetworkOutput.valueOrDefault();
    forceFirstLayerNetworkRelationFirst = config.forceFirstLayerNetworkRelationFirst.valueOrDefault();

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
    for(const auto &worker : workers)
    {
        worker->allowed_to_stop = true;
    }
    std::cout << "Waiting for workers to stop" << std::endl;
    for (const auto& worker : workers)
    {
        worker->_thread.join();
        std::cout << "Worker stopped" << std::endl;
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
        std::cout << "["<< arrivalCount++ << "] New arrival " << head.toString() << std::endl;

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
    for (const auto& inferenceTask : inferenceTasks)
    {
        if (!inferenceTask->finished && inferenceTask->net->isFinished())
        {
            // Create results obj
            nr->deactivateNetwork();
            inferenceTask->finished = true;

            // deallocate
            inferenceTask->output.policy = modeAsString;
            auto netId = inferenceTask->net->networkId;
            inferenceTask->saveNetworkResult();
            inferenceTask->dealloc();
            inferenceTask->dependents.clear();
          inferenceTask->net.reset();

//                std::cout << "DEALLOC network id: " << netId << std::endl;

            // Remove reference from list?



        }
    }


//  inferenceTasks.erase(std::remove_if(inferenceTasks.begin(), inferenceTasks.end(),
//                         [this](const std::shared_ptr<InferenceTask>& inferenceTask) {
//                             if (!inferenceTask->finished && inferenceTask->net->isFinished())
//                             {
//                               // Create results obj
//                               nr->deactivateNetwork();
//                               inferenceTask->finished = true;
//
//                               // deallocate
//                               inferenceTask->output.policy = modeAsString;
//                               auto netId = inferenceTask->net->networkId;
//                               inferenceTask->saveNetworkResult();
//                               inferenceTask->dealloc();
//
////                std::cout << "DEALLOC network id: " << netId << std::endl;
//
//                               // Remove reference from list?
//
////                               for(auto sched: this->schedulers) {
////                                 std::shared_ptr<EdgeCaffe::MasaScheduler> sched_ = std::dynamic_pointer_cast<EdgeCaffe::MasaScheduler>(sched);
////                                 std::cout << "[DEBUG] >>> size loadtasks: " << sched_->loadTasks.pool.size() << " >> size execTasks: " << sched_->execTasks.pool.size() << std::endl;
//////    std::cout << "[DEBUG] >>> tp is empty=" << taskpool->isEmpty() << std::endl;
////                               }
////
////                               std::cout << "[DEBUG] >>> size outpool=" << this->outPool->pool.size() << std::endl;
//
//
//                                return true;
//                             } else {
//                               return false;
//                             }
//
//  }), inferenceTasks.end());



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
    for (const auto& inferenceTask : inferenceTasks)
    {
        if (!inferenceTask->finished)
            finished = false;
    }
    return finished;
}

void EdgeCaffe::Orchestrator::submitInferenceTask(const EdgeCaffe::Arrival arrivalTask, bool use_scales)
{
    /**
     * @brief
     * Restructure notes
     * Keep everything in a map or vector node
     */

    std::vector<std::shared_ptr<InferenceTask>> itasks;

    for(const auto& arrivalNetwork : arrivalTask.networks)
    {
        std::shared_ptr<InferenceTask> iTask = std::make_shared<InferenceTask>();
        iTask->overideWithDataLabel = overideWithDataLabel;
        itasks.push_back(iTask);
        if(arrivalNetwork.datalabel.isSet)
        {
            iTask->dataLabel = arrivalNetwork.datalabel;
        }
        if(arrivalNetwork.relation.isSet)
        {
            iTask->relation = arrivalNetwork.relation;
        }
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
            iTask->net = std::make_shared<GeneratedNetwork>(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
//            iTask->net = new GeneratedNetwork(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
            iTask->net->init(description);
            iTask->output.networkName = iTask->net->subTasks.front()->networkName;
        } else {
            // Default network
//            iTask->net = new InferenceNetwork(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
            iTask->net = std::make_shared<InferenceNetwork>(arrivalNetwork.pathToNetwork, &this->enforceInterDependencies);
            iTask->net->init(description);
            iTask->net->use_scales = description["use-scales"].as<bool>(false);
            iTask->net->dataPath = globalConfig.pathToResources() + "/" + arrivalTask.pathToData;
            iTask->output.networkName = iTask->net->subTasks.front()->networkName;
        }
        iTask->net->meanExecutionTime = description["mean-execution-time"].as<double>(std::numeric_limits<double>::max());
        iTask->net->mc = this->mc.get();

        for(auto tp : taskPools)
            iTask->net->taskpools.push_back(tp);
        iTask->net->bagOfTasks_ptr = bagOfTasks;
        iTask->net->networkId = EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER;
        EdgeCaffe::InferenceNetwork::NETWORKID_COUNTER++;
        iTask->net->networkProfile.measure(NetworkProfile::ARRIVAL);
        inferenceTasks.push_back(iTask);
        iTask->net->createTasks(mode);
        std::vector<std::shared_ptr<Task>> listOfTasks = iTask->net->getTasks();

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

        if(mode == Type::MODE_TYPE::LINEAR || mode == Type::MODE_TYPE::EXECPRIO_INTER || mode == Type::MODE_TYPE::BULK)
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
        last->saveResults = true;


        bagOfTasks.reserve(listOfTasks.size()); // preallocate memory
        bagOfTasks.insert(bagOfTasks.end(), listOfTasks.begin(), listOfTasks.end());
//        std::cout << "[DEBUG] >>> Bag of tasks size=" << bagOfTasks.size() << std::endl;
    }

    // Create network relations
    for(auto t1: itasks)
    {
        if(t1->relation.isSet)
        {
            std::string dependsOn = t1->relation.depends;
            for(auto t2: itasks)
            {
                if(t2->net->subTasks.front()->networkName == dependsOn)
                {
                    // Depend of this network
                    // Create dependency between last and first task
                    auto dep = t2->net->subTasks.front()->lastTask;
                    auto dep_first_layer = t2->net->subTasks.front()->firstTask;


                    if(forceFirstLayerNetworkRelationFirst)
                    {
                      // Wait for the first layer of your dependent network to finish
                      if( verbose) {
                        std::cout << "[DEPENDENCY] >> Adding dep for " << t1->net->subTasks.front()->networkName
                                  << " to depend on first task of " << dep_first_layer->networkName << std::endl;
                      }
                      t1->net->subTasks.front()->firstTask->addTaskDependency(TaskDependency(dep_first_layer));
                    }

                    if(waitForNetworkFinish) {
                      // Wait for all the layers of your dependent network to finish
                      t1->net->subTasks.front()->firstTask->addTaskDependency(TaskDependency(dep));
                    }
                    t2->dependents.push_back(t1);
                }
            }
        }
    }

    // @TODO: Make sure the first layer of the network relation starts first

    // Continue network relations
    for(const auto& arrivalNetwork : arrivalTask.networks)
    {
        if(arrivalNetwork.relation.isSet)
        {
            std::string dependsOn = arrivalNetwork.relation.depends;
            for(auto it: itasks)
            {
                if(it->net->subTasks.front()->networkName == dependsOn)
                {
                    // Depend of this network
                    // Create dependency between last and first task

                }
            }

        }
    }

    if(mode == Type::MODE_TYPE::DEEPEYE || mode == Type::MODE_TYPE::DEEPEYE_FRUGAL)
    {

        int startIndex = std::max<int>(inferenceTasks.size() - arrivalTask.networks.size(), 0);
        auto it = std::begin(inferenceTasks);
        std::advance(it, startIndex);

        const auto startiTask = *it;
        auto lastConvLoad = inferenceTasks.back()->net->subTasks.front()->conv_load_last;
        auto firstConvExec = startiTask->net->subTasks.front()->conv_exec_first;
        firstConvExec->addTaskDependency(TaskDependency(lastConvLoad));

        auto previous = *it;
        ++it;

        // Keep track to the last load task for fc layers during this batch
        std::shared_ptr<Task> local_fc_load_last = nullptr;
        std::shared_ptr<Task>local_fc_exec_last = nullptr;


        for (auto end=std::end(inferenceTasks); it!=end; ++it)
        {
            const auto iTask = (*it);

            // Link init with previous load tasks
            iTask->net->subTasks.front()->conv_load_first->addTaskDependency(TaskDependency(previous->net->subTasks.front()->conv_load_last));


            // Link conv exec tasks
            iTask->net->subTasks.front()->conv_exec_first->addTaskDependency(
                    TaskDependency(previous->net->subTasks.front()->conv_exec_last));

            // Link fc load tasks
            std::shared_ptr<Task> tmp_fc_load_first = iTask->net->subTasks.front()->fc_load_first;
            std::shared_ptr<Task> tmp_fc_load_last = previous->net->subTasks.front()->fc_load_last;
            std::shared_ptr<Task>tmp_fc_exec_last = previous->net->subTasks.front()->fc_exec_last;
            // Make sure to use the last known reference is this one does not exist
            if(tmp_fc_load_last != nullptr)
                local_fc_load_last = tmp_fc_load_last;
            if(tmp_fc_exec_last != nullptr)
                local_fc_exec_last = tmp_fc_exec_last;

            // Do not try to link the task is one the reference does not exist
            if(tmp_fc_load_first != nullptr && local_fc_load_last != nullptr)
                tmp_fc_load_first->addTaskDependency(
                        TaskDependency(local_fc_load_last));
            if(mode == Type::MODE_TYPE::DEEPEYE_FRUGAL)
            {
                if (tmp_fc_load_first != nullptr && local_fc_exec_last != nullptr)
                    tmp_fc_load_first->addTaskDependency(TaskDependency(local_fc_exec_last));
            }
            previous = (*it);
        }
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
    for (const auto& inferenceTask : inferenceTasks)
    {
        std::string networkName = inferenceTask->pathToNetwork;
        std::string line = inferenceTask->output.netProfile.durationAsCSVLine(networkId, networkName, start);
        networkLines.push_back(line);
        networkId++;
    }
    output.toCSV(pathToFile, networkLines, EdgeCaffe::Output::NETWORK);
}

const std::shared_ptr<EdgeCaffe::NetworkRegistry> &EdgeCaffe::Orchestrator::getNr() const
{
    return nr;
}

const std::vector<std::shared_ptr<EdgeCaffe::InferenceTask>> &EdgeCaffe::Orchestrator::getInferenceTasks() const
{
    return inferenceTasks;
}
