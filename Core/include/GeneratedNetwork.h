//
// Created by bacox on 19/04/2020.
//

#ifndef EDGECAFFE_GENERATEDNETWORK_H
#define EDGECAFFE_GENERATEDNETWORK_H

#include "InferenceNetwork.h"

namespace EdgeCaffe
{
    struct DummyLayer
    {
        std::string name;
        long loadingDuration;
        long executionDuration;
        int id;
        std::vector<int> dependsOn;
    };
    class GeneratedNetwork : public InferenceNetwork
    {
    public:
        GeneratedNetwork(const std::string &pathToDescription);

        void createTasks(int splittingPolicy) override;

        virtual ~GeneratedNetwork();

        void init(YAML::Node &description) override;

    protected:
        std::vector<DummyLayer> layers;
    };


}


#endif //EDGECAFFE_GENERATEDNETWORK_H
