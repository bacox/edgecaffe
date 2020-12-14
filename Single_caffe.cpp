//
// Created by bacox on 22/10/2020.
//


#include <iostream>
#include <caffe/net.hpp>
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{


    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::system_clock> control;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::system_clock> wrong_end;
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto wrong_elspaed = std::chrono::duration_cast<std::chrono::nanoseconds>(wrong_end - start).count();
    auto control_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - control).count();

    std::cout <<"Duration " << elapsed << std::endl;
    std::cout <<"Worng end " << wrong_elspaed << std::endl;
    std::cout <<"Control " << control_elapsed << std::endl;

    std::cout << "Creating Caffe object" << std::endl;
    extern caffe::Net<float> *net_ptr;

    std::cout << "Ending Caffe object" << std::endl;

    return 0;
}