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
        std::chrono::time_point<std::chrono::system_clock> start_tp;

        void run();
    public:
        void start();
        void stop();

        void startTracking();
        void stopTracking();
        void join();

        int layerId = -1;
        int networkId = -1;
        int taskId = -1;
        std::string layerType = "net-init";
        std::string network = "";
        std::string taskType = "";



    };
}



#endif //EDGECAFFE_MEMCHECK_H
