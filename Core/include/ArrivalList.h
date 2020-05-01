//
// Created by bacox on 26/04/2020.
//

#ifndef EDGECAFFE_ARRIVALLIST_H
#define EDGECAFFE_ARRIVALLIST_H
#include <string>
#include <deque>

namespace EdgeCaffe
{
    struct Arrival {
        long time = 0;
        std::string pathToData;
        std::string pathToNetwork;

        std::string toString(){
            return "Arrival<time: " + std::to_string(time)+ ">";
        }
    };

    class ArrivalList
    {
        std::deque<Arrival> arrivals;

    public:
        void generateList(int numberOfArrivals);
        bool isEmpty();
        Arrival& first();
        void pop();
    };
}


#endif //EDGECAFFE_ARRIVALLIST_H
