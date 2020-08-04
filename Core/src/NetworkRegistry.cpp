//
// Created by bacox on 05/07/2020.
//

#include <iostream>
#include "../include/NetworkRegistry.h"

void EdgeCaffe::NetworkRegistry::activateNetwork()
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    activeNetworks += 1;
    updateHistory();
}

void EdgeCaffe::NetworkRegistry::deactivateNetwork()
{
// Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    activeNetworks -= 1;
    updateHistory();

}

int EdgeCaffe::NetworkRegistry::numActiveNetworks()
{
    // Use lock-guard for the mutex in the same way as a smart pointer
    // The mutex will be released when the lock-guard goes out of scope (end of function)
    std::lock_guard guard(mtx);
    return activeNetworks;
}

void EdgeCaffe::NetworkRegistry::updateHistory()
{
    history.push_back({std::chrono::high_resolution_clock::now(), activeNetworks});
}

const std::vector<EdgeCaffe::NumberNetworkProfileLine> &EdgeCaffe::NetworkRegistry::getHistory() const
{
    return history;
}

std::vector<std::string> EdgeCaffe::NetworkRegistry::toCSV()
{
    std::vector<std::string> lines;
    std::string sep = ",";
    for (auto historyLine : history){

        lines.push_back(std::to_string(historyLine.ts.time_since_epoch().count()) + sep + std::to_string(historyLine.numNetworks));
    }

     return lines;
}
