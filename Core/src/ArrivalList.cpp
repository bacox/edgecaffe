//
// Created by bacox on 26/04/2020.
//

#include <iostream>
#include <random>
#include "ArrivalList.h"
void EdgeCaffe::ArrivalList::generateList(int numberOfArrivals)
{
    // Units in miliseconds
    std::uniform_int_distribution<int> distribution(1000, 4000);
    std::default_random_engine generator;
    std::string pathToAgeNet = "networks/AgeNet";
    std::string pathToImg = "../resources/test_1.jpg";

    for( int i = 0; i < numberOfArrivals; ++i)
    {
        int delay = distribution(generator);
        arrivals.push_back(Arrival{delay, pathToImg, pathToAgeNet});
    }

    for(auto a : arrivals)
        std::cout << a.toString() << std::endl;
}

bool EdgeCaffe::ArrivalList::isEmpty()
{
    return arrivals.empty();
}

EdgeCaffe::Arrival &EdgeCaffe::ArrivalList::first()
{
    return arrivals.front();
}

void EdgeCaffe::ArrivalList::pop()
{
    arrivals.pop_front();
}
