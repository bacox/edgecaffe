//
// Created by bacox on 27-05-21.
//
#include <memory>
#include <Tasks/LoadTask.h>
#include <Tasks/ExecTask.h>
#include <Tasks/DummyTask.h>
#include "../include/InferenceTask.h"


namespace EdgeCaffe {
    void InferenceTask::applyDependencyResult(const std::string& networkName, const std::string& valuename, double result){
        bool valid = false;
        if(relation.isSet && relation.depends == networkName){
            if(relation.condition.valueName == valuename) {
                switch (relation.condition.op) {
                    case RelationOperator::GT:
                    {
//                        std::cout << "OP-GT: " << result << " > " << relation.condition.value << std::endl;
                        valid = result > relation.condition.value;
                        break;
                    }
                    case RelationOperator::LT:
                        valid = result < relation.condition.value;
                        break;
                    case RelationOperator::GEQ:
                        valid = result <= relation.condition.value;
                        break;
                    case RelationOperator::LEQ:
                        valid = result >= relation.condition.value;
                        break;
                    case RelationOperator::EQ:
                        valid = result == relation.condition.value;
                        break;
                }
            }

        }
        if(valid)
        {
            // Invalidate the tasks in this network:
//            std::cout << "Invalidate all the tasks in this network" << std::endl;
            std::cout << "Invalidate all the tasks in this network: " + this->net->subTasks.front()->networkName << std::endl;

            for(auto task : this->net->tasks){

//                if(!task || task == nullptr)
//                {
//                  std::cout << "Invalid ptr" << std::endl;
//                }
                task->invalidate();
            }
        }
    }

    void InferenceTask::saveNetworkResult(){
        if(overideWithDataLabel)
        {
//            std::cout << "Invalidate cascading networks!!" << std::endl;
            output.networkData[dataLabel.name] = dataLabel.value;


        }
        // Check dependents!!
        for(auto dependent : dependents)
        {
            dependent->applyDependencyResult(net->subTasks.front()->networkName, dataLabel.name, output.networkData[dataLabel.name]);
        }

    }
    void InferenceTask::dealloc()
    {
        net->networkProfile.measure(NetworkProfile::STOP);
        output.netProfile = net->networkProfile;

        auto ptr = net->subTasks.front()->net_ptr;
        std::vector <std::string> layerNames;
        if (ptr)
        {
            layerNames = net->subTasks.front()->net_ptr->layer_names();
        } else
        {
            for (auto t : net->tasks)
            {
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
            if (std::dynamic_pointer_cast<LoadTask>(task))
            {
//                // Load Task
                output.setLoadingTime(task);
                output.addTaskProfile(task, true);
            }
            if (std::dynamic_pointer_cast<ExecTask>(task))
            {
//                // Load Task
                output.setExecutionTime(task);
                output.addTaskProfile(task, false);
            }
            if (std::shared_ptr<DummyTask> dt = std::dynamic_pointer_cast<DummyTask>(task))
            {
                if (dt->isLoadingTask)
                {
                    output.setLoadingTime(task);
                    output.addTaskProfile(task, true);
                } else
                {
                    output.setExecutionTime(task);
                    output.addTaskProfile(task, false);
                }
            }
        }
        MemoryCounter *mc = net->mc;
        double networkMemoryUsage = net->maxMemoryUsage;
//        delete net;
//       std::cout << net->subTasks.front()->networkName <<  ": net.use_count() == " << net.use_count() << '\n';

        mc->releaseMemory(networkMemoryUsage);
    }


}
