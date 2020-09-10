//
// Created by bacox on 28/08/2020.
//

#include <iostream>
#include <Scheduler/MasaHrrnScheduler.h>
#include <Tasks/DummyTask.h>
#include <thread>
#include <chrono>
#include "gtest/gtest.h"


class HrrnTest : public ::testing::Test {
protected:
    void SetUp() override {
        scheduler = std::make_shared<EdgeCaffe::MasaHrrnScheduler>(mc, nr);
        tasks.push_back(std::make_shared<EdgeCaffe::DummyTask>(2, 0, "", 5));
        tasks.front()->t_type = EdgeCaffe::Task::EXEC;
        tasks.push_back(std::make_shared<EdgeCaffe::DummyTask>(1, 0, "", 1));
        tasks.front()->t_type = EdgeCaffe::Task::EXEC;
        tasks.push_back(std::make_shared<EdgeCaffe::DummyTask>(3, 0, "", 10));
        tasks.front()->t_type = EdgeCaffe::Task::EXEC;
        tasks.push_back(std::make_shared<EdgeCaffe::DummyTask>(4, 0, "", 100));
        tasks.front()->t_type = EdgeCaffe::Task::EXEC;
    }

    // void TearDown() override {}
    std::shared_ptr<EdgeCaffe::NetworkRegistry> nr = std::make_shared<EdgeCaffe::NetworkRegistry>();
    std::shared_ptr<EdgeCaffe::MemoryCounter> mc = std::make_shared<EdgeCaffe::MemoryCounter>();
    std::shared_ptr<EdgeCaffe::MasaHrrnScheduler> scheduler;
    std::vector<std::shared_ptr<EdgeCaffe::Task>> tasks;
};

/**
 * The list should be sorted after get next
 */
TEST_F(HrrnTest, ListShoudBeSorted) {
    for(auto t : tasks)
    {
        scheduler->addTask(t.get());
    }

    EdgeCaffe::Task *ptr;
    scheduler->getNext(&ptr);
    EXPECT_EQ (1, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (2, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (3, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (4, ptr->id);
}

/**
 * The tasks should not be sorted before use: i.e, do not sort to much
 */
TEST_F(HrrnTest, NotSortedBeforeUse)
{
    for(auto t : tasks)
        scheduler->addTask(t.get());

    int idx = 0;
    for(auto&& item : scheduler->getPool())
        // The order in the scheduler should be the same as the order of the input list.
        EXPECT_EQ(tasks[idx++]->id, *item.id);
}

/**
 * If there are no tasks, the result should be false
 */
TEST_F(HrrnTest, EmptyList)
{
    EdgeCaffe::Task * t_ptr;
    EXPECT_EQ(false, scheduler->getNext(&t_ptr));
}

/**
 * If there is only one task it should return this task
 */
TEST_F(HrrnTest, SingleTask)
{
    scheduler->addTask(tasks.front().get());
    EdgeCaffe::Task * t_ptr;
    EXPECT_EQ(true, scheduler->getNext(&t_ptr));
    EXPECT_EQ(tasks.front()->id, t_ptr->id);
}

/**
 * If no time has passed, the order should be SJF
 */
TEST_F(HrrnTest, NoTimePast)
{
    for(const auto& t : tasks)
        scheduler->addTask(t.get());
    EdgeCaffe::Task *ptr;
    scheduler->getNext(&ptr);
    EXPECT_EQ (1, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (2, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (3, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (4, ptr->id);
}

/**
 * When we wait long enough the long task should get priority
 */
TEST_F(HrrnTest, LongFirst)
{
    scheduler->addTask(tasks[3].get());
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
//    // Make sure the passed time has been registered and processed.
//    scheduler->calcElapsedTime();
//    scheduler->updateTime();
    scheduler->addTask(tasks[0].get());
    scheduler->addTask(tasks[1].get());
    scheduler->addTask(tasks[2].get());

    EdgeCaffe::Task *ptr;
    scheduler->getNext(&ptr);
    EXPECT_EQ (4, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (1, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (2, ptr->id);
    scheduler->getNext(&ptr);
    EXPECT_EQ (3, ptr->id);

}


/**
 * When we wait for a short period, the order won't change.
 * Task_id | exec_time | waiting_time | response_ratio |
 * =====================================================
 * Task 1  | 1 ms      | 1ms           | 2             |
 * Task 2  | 5 ms      | 1ms           | 1.2           |
 * Task 3  | 10 ms     | 1ms           | 1.1           |
 * Task 4  | 100 ms    | 6ms          | 1.054          |
 */
TEST_F(HrrnTest, ShortKeepOrder)
{
    scheduler->addTask(tasks[3].get());
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
//    // Make sure the passed time has been registered and processed.
//    scheduler->calcElapsedTime();
//    scheduler->updateTime();
    scheduler->addTask(tasks[0].get());
    scheduler->addTask(tasks[1].get());
    scheduler->addTask(tasks[2].get());


    scheduler->sortTasks();
    auto sortedTasks = scheduler->getPool();

    EXPECT_EQ(1, *(sortedTasks[0].id));
    EXPECT_EQ(2, *(sortedTasks[1].id));
    EXPECT_EQ(3, *(sortedTasks[2].id));
    EXPECT_EQ(4, *(sortedTasks[3].id));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}