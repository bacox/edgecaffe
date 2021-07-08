//
// Created by bacox on 27/07/2020.
//

#ifndef EDGECAFFE_INFERENCETASK_H
#define EDGECAFFE_INFERENCETASK_H

#include <InferenceOutput.h>
#include "InferenceNetwork.h"
#include "NetworkRelation.h"

namespace EdgeCaffe
{
    /**
     * Stucture to hold the references and the data of the network, tasks and the layers during execution
     */
    struct InferenceTask
    {
        std::string pathToNetwork;
        std::string pathToData;
        std::shared_ptr<InferenceNetwork> net;
        NetworkRelation relation;
        std::vector<std::shared_ptr<InferenceTask>> dependents;
        DataLabel dataLabel;
        bool overideWithDataLabel{false};


        bool finished = false;

        InferenceOutput output;
        void applyDependencyResult(const std::string& networkName, const std::string& valuename, double result);
        void saveNetworkResult();
        void dealloc();
    };
}
#endif //EDGECAFFE_INFERENCETASK_H
