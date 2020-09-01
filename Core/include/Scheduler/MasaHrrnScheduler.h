//
// Created by bacox on 28/08/2020.
//

#ifndef EDGECAFFE_MASAHRRNSCHEDULER_H
#define EDGECAFFE_MASAHRRNSCHEDULER_H

#include "Scheduler.h"

namespace EdgeCaffe
{

    class MasaHrrnScheduler : public Scheduler
    {
    public:
        class HrrnItem {
        public:
            Task::TYPE type;
            int id;
            HrrnItem(Task::TYPE type, int id, int serviceTime, Task *tPtr, int waitingTime = 1, float rr = 0) : type(type),
                                                                                                        id(id),
                                                                                                        serviceTime(
                                                                                                                serviceTime
                                                                                                        ), t_ptr(tPtr),
                                                                                                        waitingTime(
                                                                                                                waitingTime
                                                                                                        ), rr(rr)
            {}

            int serviceTime = 0;
//            int *elapsed_time = nullptr;
            Task *t_ptr = nullptr;
            int
            waitingTime = 1; // Use initial wait time of 1ms to ensure that SJF works even when no time has passed
            float rr = 0; //Response ratio

            void updateRR(int elapsedTime)
            {
                waitingTime += elapsedTime;
                rr = 1.f + ((float)waitingTime / (float)serviceTime );
            }
            bool operator<(const HrrnItem& b) const {
                return rr < b.rr;
            }

            bool operator>(const HrrnItem& b) const {
                return rr > b.rr;
            }

        };
    private:
        std::vector<HrrnItem> pool;
        int elapsed_time = 0;
        std::chrono::time_point<std::chrono::system_clock> last_tp;
    public:
        void setElapsedTime(int elapsedTime);

    public:
        void addTask(Task *t_ptr) override;

        void updateTime();

        void calcElapsedTime();


        bool isEmpty() override;

        void sortTasks();

        bool getNext(Task **t_ptr) override;

        MasaHrrnScheduler(const std::shared_ptr<MemoryCounter> &mc, const std::shared_ptr<NetworkRegistry> &nr);

        const std::vector<HrrnItem> &getPool() const;
    };
}


#endif //EDGECAFFE_MASAHRRNSCHEDULER_H
