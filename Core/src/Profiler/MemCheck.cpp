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
    prev_tp = start_tp;
    while(running)
    {
        if(tracking)
        {
            const auto mem = getRSS();
            numChecks++;
            auto now2 = std::chrono::high_resolution_clock::now();
            const auto tmp_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now2 - prev_tp).count();
            measurements.push_back(tmp_duration);
            prev_tp = now2;

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
    end_tp = std::chrono::high_resolution_clock::now();
    const auto total_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    double sum = std::accumulate(measurements.begin(), measurements.end(), 0.0);
    double mean = sum / measurements.size();

    std::vector<double> diff(numChecks);
    std::transform(measurements.begin(), measurements.end(), diff.begin(),
            std::bind2nd(std::minus<double>(), mean));
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / numChecks);

    std::cout << "Memcheck resolution is " << total_duration / numChecks << " nanoseconds between measurements. Count=" << numChecks << std::endl;
    std::cout << "Mean measuering time is " << mean << " and the st-dev is " << stdev << " count=" << sum << std::endl;
    std::cout << "Mem check stopped." << std::endl;

    std::ofstream myfile;
    myfile.open ("rss_speed.csv");
    for(auto m : measurements)
    {
        myfile << m << "\n";
    }
    myfile.close();
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
    measurements.reserve(5000000);
}
