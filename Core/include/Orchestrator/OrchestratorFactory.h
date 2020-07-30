//
// Created by jeroen on 5/5/20.
//

#ifndef EDGECAFFE_ORCHESTRATORFACTORY_H
#define EDGECAFFE_ORCHESTRATORFACTORY_H


#include <map>
#include <typeindex>
//#include <utils/config.h>
//#include <TaskPool/LJFTaskPool.h>
#include <Util/Config.h>
#include "Orchestrator.h"
//#include "LinearOrchestrator.h"
//#include "DeepEyeOrchestrator.h"
#include "BulkOrchestrator.h"
//#include "PartialOrchestrator.h"
//#include "LongestProcessingTimeOrchestrator.h"
#include "MasaOrchestrator.h"
#include "DeepEyeOrchestrator.h"

namespace EdgeCaffe {


    class OrchestratorFactory {

    private:

        typedef void (*voidFunctionType)();

        std::map<Type::MODE_TYPE, std::pair<voidFunctionType, std::type_index>> orchestratorMap;

    public:

        /**
         * @brief Constructor for singleton instance.
         */
        OrchestratorFactory(const OrchestratorFactory &) = delete;

        /**
         * @brief Destructor, clears the orchestrator map with references to types of Orchestrators.
         **/
        ~OrchestratorFactory() { orchestratorMap.clear(); }

        static OrchestratorFactory *Get() {
            static OrchestratorFactory instance;
            return &instance;
        }

        /**
         * @brief Function to register an Orchestrator type to the factory.
         * @tparam Type of Orchestrator to store, corresponding to the orchestratorName argument.
         * @param orchestratorName OrchestratorBase::MODEL_SPLIT_MODE to store the type of Orchestrator in the
         * orchestratorMap.
         * @param pointer Reference to Create function of an Orchestrator Object/Class.
         */
        template<class A>
        void Register(const Type::MODE_TYPE orchestratorName, A pointer) {
            auto tt = std::type_index(typeid(pointer));
            orchestratorMap.insert(std::make_pair(orchestratorName,
                                                  std::make_pair((voidFunctionType) pointer, tt)));
        }

        /**
         * @brief Function to retrieve a specific orchestrator type.
         * @tparam Args List of arguments to pass to the construction of an Orchestrator.
         * @param orchestratorName MODEL_SPLIT_MODE indicating which kind of orchestrator is requested.
         * @param args Optional arguments to be passed to the construction of an Orchestrator.
         * @return
         */
//        template<typename... Args>
//        std::shared_ptr<Orchestrator> GetOrchestrator(Args &&... args) {
//            auto &runtimeConfig = EdgeCaffe::Config::getInstance();
//
//            auto modus = runtimeConfig.modeAsType();
//            auto it = orchestratorMap.find(modus);
//            if (it == orchestratorMap.end()) {
//                std::cout << "Not implemented or invalid orchestrator type was requested, exiting..." << std::endl;
//                exit(-1);
//            }
//            auto mapValue = it->second;
//            auto casted = (Orchestrator(*)(Args...)) (mapValue.first);
//            return casted(std::forward<Args>(args)...);
//        }

        std::shared_ptr<Orchestrator> GetOrchestratorAlt()
        {
            auto &runtimeConfig = EdgeCaffe::Config::getInstance();

            auto modus = runtimeConfig.modeAsType();
            switch (modus)
            {
                case Type::MODE_TYPE::BULK:
                    return EdgeCaffe::BulkOrchestrator::Create();
                case Type::MODE_TYPE::MASA:
                    return EdgeCaffe::MasaOrchestrator::Create();
                case Type::MODE_TYPE::DEEPEYE:
                    return EdgeCaffe::DeepEyeOrchestrator::Create();
                default:
                    std::cout << "Not implemented or invalid orchestrator type was requested, exiting..." << std::endl;
                    exit(-1);
            }
        }

        /**
         * @brief Constructor for OrchestratorFactory, initializes by populating the reference map.
         */
        OrchestratorFactory() {
            Register(Type::MODE_TYPE::DEEPEYE, EdgeCaffe::DeepEyeOrchestrator::Create);
//            Register(Type::MODE_TYPE::LINEAR, EdgeCaffe::LinearOrchestrator::Create);
            Register(Type::MODE_TYPE::BULK, EdgeCaffe::BulkOrchestrator::Create);
//            Register(Type::MODE_TYPE::PARTIAL, EdgeCaffe::PartialOrchestrator::Create);};
            Register(Type::MODE_TYPE::MASA, EdgeCaffe::MasaOrchestrator::Create);};
    };

//
//    class Builder {
//
//
//    public:
//
//        OrchestratorFactory<FCFSTaskPool> *fcfs_factory;
//        OrchestratorFactory<LJFTaskPool> *ljf_factory;
//        OrchestratorFactory<SJFTaskPool> *sjf_factory;
//
//        OrchestratorBase::MODEL_SPLIT_MODE mode;
//
//        void setMode(OrchestratorBase::MODEL_SPLIT_MODE mode_) {
//            mode = mode_;
//        }
//
//        Orchestrator f getOrchestrator
//    };


}


#endif //EDGECAFFE_ORCHESTRATORFACTORY_H
