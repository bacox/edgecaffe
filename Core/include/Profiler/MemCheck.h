//
// Created by bacox on 16/05/2020.
//

#ifndef EDGECAFFE_MEMCHECK_H
#define EDGECAFFE_MEMCHECK_H

#include <fstream>
#include <chrono>

namespace EdgeCaffe
{
    class MemCheck
    {
        double getRSS();
        bool running = true;
        bool tracking = false;
        std::thread _thread;
        std::ofstream filebuffer;
        std::string file_path;
        std::chrono::time_point<std::chrono::system_clock> start_tp;
        std::chrono::time_point<std::chrono::system_clock> end_tp;
        std::size_t numChecks = 0;
        double latestMeasurement = 0;
        double latestTaskId = -1;
        std::vector<double> measurements;
        std::chrono::time_point<std::chrono::system_clock> prev_tp;

        void run();
    public:
        void start();

        MemCheck();

        void stop();

        void startTracking();
        void stopTracking();
        void join();

        int layerId = -1;
        int networkId = -1;
        int taskId = -1;
        int sign = 0;
        std::string layerType = "net-init";
        std::string network = "";
        std::string taskType = "";



    };
}



#endif //EDGECAFFE_MEMCHECK_H
