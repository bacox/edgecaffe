//
// Created by bacox on 20/09/2020.
//

#include <fstream>
#include <iostream>
#include "../../include/Profiler/Memory.h"

double EdgeCaffe::Memory::getPageFailtFromOS()
{
    std::ifstream buffer("/proc/self/stat");

    Pidstat stat;

    stat.parseBUffer(buffer);
//    std::cout << "Number of minor page faults: " << stat.minflt_ << std::endl;
//    std::cout << "Number of major page faults: " << stat.majflt_ << std::endl;
    return stat.majflt_;
//    return 0;
}

double EdgeCaffe::Memory::numMajPageFaults()
{
    return getPageFailtFromOS();
}

void EdgeCaffe::Pidstat::parseBUffer(std::ifstream &buffer)
{
    buffer >> pid_ >> filename_ >> state_ >> ppid_ >>  pgrp_ >> session_
           >> tty_nr_ >> tpgid_ >> flags_ >> minflt_ >> cminflt_ >> majflt_
           >> cmajflt_ >> utime_ >> stime_ >> cutime_ >> cstime_ >> priority_
           >> nice_ >> num_threads_ >> itrealvalue_ >> starttime_ >> vsize_
           >> rss_ >> rlim_ >> startcode_ >> endcode_ >> startstack_ >> kstkesp_
           >> kstkeip_ >> signal_ >> blocked_ >> sigignore_ >>  sigcatch_ >> wchan_
           >> nswap_ >> cnswap_ >> exit_signal_ >> processor_ >> rt_priority_
           >> policy_ >> delayacct_blkio_ticks_ >> guest_time_ >> cguest_time_;
}
