//
// Created by bacox on 30/03/2020.
//

#ifndef CPU_ONLY
#define CPU_ONLY
#endif

#include <iostream>
//#include "core/InferenceNetwork.h"
//#include "core/TaskPool.h"
//#include "core/Worker.h"
#include <opencv2/imgcodecs.hpp>
#include <thread>
#include <InferenceNetwork.h>
#include <TaskPool.h>
#include <Worker.h>


int main(int argc, char *argv[]) {

    std::cout << "Scheduled Pipeline" << std::endl;



    /**
     * (1). Load GenderNet into InferenceNetwork
     * (2). Grab simple image as input
     * (3). Run "normal" inference as done in Network2Task
     * (4). Call overridable method showResult ?
     */

    std::string pathToNetDescr = "networks/GenderNet";
    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToImg = "images/narendra_modi.jpg";
    cv::Mat input_img = cv::imread(pathToImg);


    InferenceNetwork genderNet(pathToNetDescr);
    genderNet.init();
    InferenceNetwork ageNet(pathToAgeNet);
    ageNet.init();

    genderNet.setInput(input_img, false);
    genderNet.loadNetworkStructure();
    genderNet.loadInputToNetwork();

    ageNet.setInput(input_img, false);
    ageNet.loadNetworkStructure();
    ageNet.loadInputToNetwork();

    genderNet.createTasks();
    ageNet.createTasks();

    std::vector<Task*> genderNetTasks = genderNet.getTasks();
    std::vector<Task*> ageNetTasks = ageNet.getTasks();

    std::vector<Task*> bagOfTasks;
    bagOfTasks.reserve( genderNetTasks.size() + ageNetTasks.size() ); // preallocate memory
    bagOfTasks.insert( bagOfTasks.end(), genderNetTasks.begin(), genderNetTasks.end() );
    bagOfTasks.insert( bagOfTasks.end(), ageNetTasks.begin(), ageNetTasks.end() );

    TaskPool pool;

    TaskPool outPool;

    std::vector<Worker> workers;
    int numberOfWorkers = 1;
    for(int i = 0; i < numberOfWorkers; ++i) {
        std::cout << "Creating new worker[" << i << "]" << std::endl;
        workers.push_back(Worker(&pool, &outPool));
    }

    for(Worker &w: workers){
        w.run();
    }

    while(bagOfTasks.size() > 0)
    {
        for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++) {
            // remove odd numbers
            Task *task = *it;
            if(!task->waitsForOtherTasks()){
                bagOfTasks.erase(it--);
                pool.addTask(task);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for(Worker &w: workers)
    {
        w.allowed_to_stop = true;
    }
    std::cout << "Waiting for worker to stop running" << std::endl;
    for(Worker &w : workers)
    {
        w._thread.join();
    }
    std::cout << "End of task pool run" << std::endl;

    genderNet.showResult();
    ageNet.showResult();

    return 0;
}