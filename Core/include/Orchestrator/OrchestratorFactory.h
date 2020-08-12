//
// Created by jeroen on 5/5/20.
//

#ifndef EDGECAFFE_ORCHESTRATORFACTORY_H
#define EDGECAFFE_ORCHESTRATORFACTORY_H


#include <map>
#include <typeindex>
#include <Util/Config.h>
#include "Orchestrator.h"
#include "BulkOrchestrator.h"
#include "MasaOrchestrator.h"
#include "DeepEyeOrchestrator.h"
#include "LinearOrchestrator.h"

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
                case Type::MODE_TYPE::DEEPEYE_FRUGAL:
                    return EdgeCaffe::DeepEyeOrchestrator::Create();
                case Type::MODE_TYPE::LINEAR:
                    return EdgeCaffe::LinearOrchestrator::Create();
                default:
                    std::cout << "Not implemented or invalid orchestrator type was requested, exiting..." << std::endl;
                    exit(-1);
            }
        }

        /**
         * @brief Constructor for OrchestratorFactory, initializes by populating the reference map.
         */
        OrchestratorFactory()
        {
            Register(Type::MODE_TYPE::DEEPEYE, EdgeCaffe::DeepEyeOrchestrator::Create);
            Register(Type::MODE_TYPE::BULK, EdgeCaffe::BulkOrchestrator::Create);
            Register(Type::MODE_TYPE::MASA, EdgeCaffe::MasaOrchestrator::Create);
            Register(Type::MODE_TYPE::LINEAR, EdgeCaffe::LinearOrchestrator::Create);
        }
    };
}


#endif //EDGECAFFE_ORCHESTRATORFACTORY_H
