//
// Created by bacox on 16/05/2020.
//

#include <thread>
#include <iostream>
#include <unistd.h>
#include "../../include/Profiler/MemCheck.h"

double EdgeCaffe::MemCheck::getRSS()
{
    int tSize = 0, resident = 0, share = 0;
    std::ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    double rss = resident * page_size_kb;

    return rss;
}

void EdgeCaffe::MemCheck::run()
{
     filebuffer.open("../analysis/allocations.csv", std::ios::out);

    filebuffer << "time,rss_kb,networkId,layerId,taskId,layerType,network,taskType" << std::endl;
    start_tp = std::chrono::high_resolution_clock::now();
    while(running)
    {
        if(tracking)
        {
            const auto mem = getRSS();
//            double mem = 0;
            const auto tp = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(tp - start_tp).count();
            filebuffer << duration << "," << mem << "," << networkId  << "," << layerId  << "," << taskId << "," << layerType << "," << network << "," << taskType << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::nanoseconds (1));
    }
    filebuffer.close();
    std::cout << "Mem check stopped." << std::endl;
}

void EdgeCaffe::MemCheck::start()
{
    _thread = std::thread(&MemCheck::run, this);
}

void EdgeCaffe::MemCheck::startTracking()
{
    tracking = true;
}

void EdgeCaffe::MemCheck::stopTracking()
{
    tracking = false;
}

void EdgeCaffe::MemCheck::stop()
{
    running = false;
}

void EdgeCaffe::MemCheck::join()
{
    _thread.join();
}
