//
// Created by bacox on 16/05/2020.
//

#include <thread>
#include <iostream>
#include <unistd.h>
#include <Util/Config.h>
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
//     filebuffer.open("../analysis/allocations.csv", std::ios::out);
     filebuffer.open(file_path, std::ios::out);

    filebuffer << "time,rss_kb,networkId,layerId,taskId,layerType,network,taskType,diff-sign" << std::endl;
    start_tp = std::chrono::high_resolution_clock::now();
    while(running)
    {
        if(tracking)
        {
            const auto mem = getRSS();
            if(mem == latestMeasurement && taskId == latestTaskId)
                continue;
            double tmp_sign = latestMeasurement - mem;
            tmp_sign = (tmp_sign > 0) - (tmp_sign < 0);
            latestMeasurement = mem;
            latestTaskId = taskId;
            if(tmp_sign == sign)
                continue;
            sign = tmp_sign;
//            double mem = 0;
            const auto tp = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(tp - start_tp).count();
            filebuffer << duration << "," << mem << "," << networkId  << "," << layerId  << "," << taskId << "," << layerType << "," << network << "," << taskType << "," << std::to_string(sign) << std::endl;
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

EdgeCaffe::MemCheck::MemCheck()
{
    auto &config = Config::getInstance();
    file_path = config.outputDirectory() + "/" + "allocations.csv";
}
