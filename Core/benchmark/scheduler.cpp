//
// Created by bacox on 01/09/2020.
//
#include <iostream>
#include <Tasks/Task.h>
#include <Tasks/DummyTask.h>
#include <random>
#include <Scheduler/Scheduler.h>
#include <Scheduler/MasaHrrnScheduler.h>
#include <Scheduler/FCFSScheduler.h>
#include <Scheduler/MasaScheduler.h>
#include <Scheduler/MasaHrrnLLScheduler.h>
#include <Scheduler/MasaSchedulerWithItem.h>

struct SchedulerDescription
{
    std::string description;
    std::shared_ptr<EdgeCaffe::Scheduler> scheduler;
};
void generate_tasks(int numTasks, std::vector<std::shared_ptr<EdgeCaffe::Task>>& listOfTasks)
{
    int idx = 0;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(1000.0,400.0);

    while(idx < numTasks)
    {
        listOfTasks.emplace_back(std::make_shared<EdgeCaffe::DummyTask>(1, 0, "", distribution(generator)));
        listOfTasks.back()->t_type = EdgeCaffe::Task::LOAD;
        idx++;
    }
}

void benchmark_scheduler(std::vector<std::shared_ptr<EdgeCaffe::Task>>& listOfTasks, SchedulerDescription &sd)
{

    std::shared_ptr<EdgeCaffe::Scheduler> scheduler = sd.scheduler;


    for(auto task : listOfTasks)
    {
        auto *ptr = new std::vector<int>();
        scheduler->addTask(task);
        delete ptr;
    }
    std::shared_ptr<EdgeCaffe::Task> next;
    int idx = 0;
    int total = listOfTasks.size();


    auto start_tp = std::chrono::high_resolution_clock::now();
    while(!scheduler->isEmpty())
    {
        auto *ptr = new std::vector<int>();
        scheduler->getNext(&next);
        delete ptr;
//        std::cout << "Items left \t" << (count - idx++) << std::endl;
    }
    auto end_tp = std::chrono::high_resolution_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    std::cout << "[ " << sd.description << " ] \tDuration is " << elapsed_time << " ns" << std::endl;
    std::cout << "[ " << sd.description << " ] \tDuration is " << (elapsed_time / total) << " ns per task" << std::endl;


}

void benchmark_scheduler_batch(std::vector<std::shared_ptr<EdgeCaffe::Task>>& listOfTasks, SchedulerDescription &sd, int batchSize = 10)
{
    std::shared_ptr<EdgeCaffe::Scheduler> scheduler = sd.scheduler;

    int idx = 0;
    int total = listOfTasks.size();
    std::shared_ptr<EdgeCaffe::Task> next;
    auto start_tp = std::chrono::high_resolution_clock::now();

    while(idx < total)
    {
        for(int i = 0; i < batchSize; ++i)
        {
            if(idx == total)
                continue;
            auto *ptr = new std::vector<int>();
            scheduler->addTask(listOfTasks[idx++]);
            delete ptr;
        }
        while(!scheduler->isEmpty())
            scheduler->getNext(&next);
    }
    auto end_tp = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    std::cout << "[ " << sd.description << " ] \tDuration is " << elapsed_time << " ns" << std::endl;
    std::cout << "[ " << sd.description << " ] \tDuration is " << (elapsed_time / total) << " ns per task" << std::endl;
}

void benchmark_addTask(std::vector<std::shared_ptr<EdgeCaffe::Task>>& listOfTasks, SchedulerDescription &sd)
{
    std::shared_ptr<EdgeCaffe::Scheduler> scheduler = sd.scheduler;
    int total = listOfTasks.size();

    auto start_tp = std::chrono::high_resolution_clock::now();
    for(auto task : listOfTasks)
    {
        scheduler->addTask(task);
    }
    auto end_tp = std::chrono::high_resolution_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    std::cout << "[ " << sd.description << " ] \tDuration is " << elapsed_time << " ns" << std::endl;
    std::cout << "[ " << sd.description << " ] \tDuration is " << (elapsed_time / total) << " ns per task" << std::endl;
}


void benchmark_getNet(std::vector<std::shared_ptr<EdgeCaffe::Task>>& listOfTasks, SchedulerDescription &sd)
{
    std::shared_ptr<EdgeCaffe::Scheduler> scheduler = sd.scheduler;
    int total = listOfTasks.size();
    std::shared_ptr<EdgeCaffe::Task> next;
    auto start_tp = std::chrono::high_resolution_clock::now();
    for(auto task : listOfTasks)
    {
        scheduler->addTask(task);
    }

    while(!scheduler->isEmpty())
    {
//        auto *ptr = new std::vector<int>();
        scheduler->getNext(&next);
//        delete ptr;
//        std::cout << "Items left \t" << (count - idx++) << std::endl;
    }

    auto end_tp = std::chrono::high_resolution_clock::now();

    auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp).count();

    std::cout << "[ " << sd.description << " ] \tDuration is " << elapsed_time << " ns" << std::endl;
    std::cout << "[ " << sd.description << " ] \tDuration is " << (elapsed_time / total) << " ns per task" << std::endl;
}





bool isTrue(){
    return true;
}


bool elvis(){
    return isTrue() ?: false;
}

int main(int argc, char *argv[]) {
    std::cout << "Benchmark code" << std::endl;

    std::vector<std::shared_ptr<EdgeCaffe::Task>> listOfTasks;
    std::vector<SchedulerDescription> schedulers;
    schedulers.push_back({
            "FCFS",
            std::make_shared<EdgeCaffe::FCFSScheduler>(
                    std::make_shared<EdgeCaffe::MemoryCounter>(),
                    std::make_shared<EdgeCaffe::NetworkRegistry>()
            )
        });
    schedulers.push_back({
             "Masa",
             std::make_shared<EdgeCaffe::MasaScheduler>(
                     std::make_shared<EdgeCaffe::MemoryCounter>(),
                     std::make_shared<EdgeCaffe::NetworkRegistry>()
             )
     });
    schedulers.push_back({
                                 "Masa With Item",
                                 std::make_shared<EdgeCaffe::MasaSchedulerWithItem>(
                                         std::make_shared<EdgeCaffe::MemoryCounter>(),
                                         std::make_shared<EdgeCaffe::NetworkRegistry>()
                                 )
                         });
    schedulers.push_back({
                                 "Masa HRRN Vector",
                                 std::make_shared<EdgeCaffe::MasaHrrnScheduler>(
                                         std::make_shared<EdgeCaffe::MemoryCounter>(),
                                         std::make_shared<EdgeCaffe::NetworkRegistry>()
                                 )
                         });
    schedulers.push_back({
                                 "Masa HRRN LL",
                                 std::make_shared<EdgeCaffe::MasaHrrnLLScheduler>(
                                         std::make_shared<EdgeCaffe::MemoryCounter>(),
                                         std::make_shared<EdgeCaffe::NetworkRegistry>()
                                 )
                         });

    int numberOfTasks = 10000;
    generate_tasks(numberOfTasks, listOfTasks);
    std::cout << "==[ Single list of " << numberOfTasks << " tasks ]==" << std::endl;
    for(auto scheduler : schedulers)
        benchmark_scheduler(listOfTasks, scheduler);

    std::vector<int> batchSizes = {1,2,5,10,20,50,100, 200, 500, 1000};

    for(auto batchSize : batchSizes)
    {
        std::cout << "==[ Batch of " << batchSize << " in list of " << numberOfTasks << " tasks ]==" << std::endl;
        for(auto scheduler : schedulers)
            benchmark_scheduler_batch(listOfTasks, scheduler, batchSize);
    }

    std::cout << "==[ AddTask benchmark with " << numberOfTasks << " tasks ]==" << std::endl;
    for(auto scheduler : schedulers)
        benchmark_addTask(listOfTasks, scheduler);

    std::cout << "==[ GetNext benchmark with " << numberOfTasks << " tasks ]==" << std::endl;
    for(auto scheduler : schedulers)
        benchmark_getNet(listOfTasks, scheduler);

    return 0;
}