//
// Created by bacox on 03/07/2020.
//

#ifndef EDGECAFFE_MEMORYCOUNTER_H
#define EDGECAFFE_MEMORYCOUNTER_H

#include <mutex>

namespace EdgeCaffe{
    /**
     * MemoryCounter keeps track of the memory within the system
     */
    class MemoryCounter
    {
    private:
        double capacity = 0;
        double freeSpace = 0;


    protected:
        mutable std::mutex mtx;

    public:

        bool * interNetworkCondition;

        MemoryCounter(double capacity = 0);

        double getCapacity() const;

        void setCapacity(double capacity);

        /**
         * Returns the absolute free space value, hence is always a non-negative value
         * @return
         */
        double getFreeSpace() const;

        /**
         * Returns the free space value. NOTE: this value can be negative.
         * @return
         */
        double getRealFreeSpace() const;

        void lockMemory(double usage);
        void releaseMemory(double usage);
    };
}


#endif //EDGECAFFE_MEMORYCOUNTER_H
