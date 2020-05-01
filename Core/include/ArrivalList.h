//
// Created by bacox on 26/04/2020.
//

#ifndef EDGECAFFE_ARRIVALLIST_H
#define EDGECAFFE_ARRIVALLIST_H
#include <string>
#include <deque>

namespace EdgeCaffe
{
    /**
     * Data structure used in the ArrivalList to describe an Arrival
     * It hold a location to input data and the location of the network to be used.
     */
    struct Arrival {
        long time = 0;
        std::string pathToData;
        std::string pathToNetwork;

        std::string toString(){
            return "Arrival<time: " + std::to_string(time)+ ">";
        }
    };

    /**
     * Arrival list can be used to generate a list of arrivals based on a stochastic distribution.
     * Currently the uniform distribution is provided.
     *
     */
    class ArrivalList
    {
        // Queue structure to hold the variables.
        std::deque<Arrival> arrivals;

    public:
        /**
         * Generate a `numberOfArrivals` based on a stochastic distribution
         * The distribution will generate the time between arrivals.
         * So the time is the relative time in relation to the previous arrival, not the absolute time.
         * @param numberOfArrivals  Int - The number of arrivals to be generated
         */
        void generateList(int numberOfArrivals);
        bool isEmpty();

        /**
         * Gets the next arrival
         * @return Arrival object
         *
         * Usage:
         *  ArrivalList list;
         *  auto nextArrival = list.first();
         *  list.pop();
         */
        Arrival& first();

        /**
         * Removes the first element of the queue
         */
        void pop();
    };
}


#endif //EDGECAFFE_ARRIVALLIST_H
