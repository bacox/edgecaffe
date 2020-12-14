//
// Created by bacox on 05/07/2020.
//

#ifndef EDGECAFFE_NETWORKREGISTRY_H
#define EDGECAFFE_NETWORKREGISTRY_H

#include <mutex>
#include <vector>
#include <chrono>

namespace EdgeCaffe
{
    struct NumberNetworkProfileLine
    {
        std::chrono::time_point<std::chrono::system_clock> ts;
        int numNetworks = 0;
    };

    class NetworkRegistry
    {

    private:
        std::mutex mtx;
        int activeNetworks = 0;

        std::vector<NumberNetworkProfileLine> history = {{std::chrono::high_resolution_clock::now(), activeNetworks}};

        void updateHistory();

    public:
        void activateNetwork();

        int numActiveNetworks();

        void deactivateNetwork();

        const std::vector<NumberNetworkProfileLine> &getHistory() const;

        std::vector<std::string> toCSV();
    };

}

#endif //EDGECAFFE_NETWORKREGISTRY_H
