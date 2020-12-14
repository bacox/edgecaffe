//
// Created by bacox on 30/03/2020.
//

#ifndef CPU_ONLY
#define CPU_ONLY
#endif

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <thread>
#include <EdgeCaffe.h>



int main(int argc, char *argv[])
{
    std::cout << "Scheduled Pipeline" << std::endl;

    /**
     * This is a basic example to execute partial networks
     * Two networks are used: AgeNet and GenderNet
     * NOTE: The paths are hardcoded in this case!
     *       In some cases these paths needs to be corrected!
     *
     * As glog is not initialized all the debug output of caffe is written to stdout as well
     *
     * (1). Load GenderNet into InferenceNetwork
     * (2). Grab simple image as input
     * (3). Run "normal" inference as done in Network2Task
     * (4). Call overridable method showResult ?
     */

    std::string pathToNetDescr = "networks/GenderNet";
    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToImg = "../resources/test_1.jpg";
    cv::Mat input_img = cv::imread(pathToImg);


    EdgeCaffe::InferenceNetwork genderNet(pathToNetDescr);
    genderNet.init();
    EdgeCaffe::InferenceNetwork ageNet(pathToAgeNet);
    ageNet.init();

    /**
     * Load the input image to the first layer of gendernet
     */
    genderNet.setInput(input_img, false);
    genderNet.loadNetworkStructure();
    genderNet.loadInputToNetwork();

    /**
     * Load the input image to the first layer of agenet
     */
    ageNet.setInput(input_img, false);
    ageNet.loadNetworkStructure();
    ageNet.loadInputToNetwork();

    // Create all the tasks with their dependencies
    genderNet.createTasks(1);
    ageNet.createTasks(1);

    /**
     * Combine the generated tasks into one single vector of tasks
     * This is done by the Orchestrator class when an orchestrator is used.
     */
    std::vector<EdgeCaffe::Task *> genderNetTasks = genderNet.getTasks();
    std::vector<EdgeCaffe::Task *> ageNetTasks = ageNet.getTasks();
    std::vector<EdgeCaffe::Task *> bagOfTasks;
    bagOfTasks.reserve(genderNetTasks.size() + ageNetTasks.size()); // preallocate memory
    bagOfTasks.insert(bagOfTasks.end(), genderNetTasks.begin(), genderNetTasks.end());
    bagOfTasks.insert(bagOfTasks.end(), ageNetTasks.begin(), ageNetTasks.end());

    // Create the ready task pool
    EdgeCaffe::TaskPool pool;
    // Create the finished task pool
    EdgeCaffe::TaskPool outPool;

    /**
     * Initialize worker
     */
    std::vector<EdgeCaffe::Worker> workers;
    int numberOfWorkers = 1;
    for (int i = 0; i < numberOfWorkers; ++i)
    {
        std::cout << "Creating new worker[" << i << "]" << std::endl;
        workers.push_back(EdgeCaffe::Worker(&pool, &outPool, i));
    }

    // Start the workers
    for (EdgeCaffe::Worker &w: workers)
    {
        w.run();
    }

    /**
     * Processing the tasks generated by the networks
     */
    while (bagOfTasks.size() > 0)
    {
        for (auto it = bagOfTasks.begin(); it != bagOfTasks.end(); it++)
        {
            // remove odd numbers
            EdgeCaffe::Task *task = *it;
            if (!task->waitsForOtherTasks())
            {
                bagOfTasks.erase(it--);
                pool.addTask(task);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Wait for the workers to end before continuing
    for (EdgeCaffe::Worker &w: workers)
    {
        w.allowed_to_stop = true;
    }
    std::cout << "Waiting for worker to stop running" << std::endl;
    for (EdgeCaffe::Worker &w : workers)
    {
        w._thread.join();
    }

    // Done
    std::cout << "End of task pool run" << std::endl;

    // Show the original input and print the estimated result.
    // Press any key to continue
    genderNet.showResult();
    ageNet.showResult();

    return 0;
}
