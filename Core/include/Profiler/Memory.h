//
// Created by bacox on 20/09/2020.
//

#ifndef EDGECAFFE_MEMORY_H
#define EDGECAFFE_MEMORY_H

namespace EdgeCaffe
{
    class Pidstat
    {
    public:
        int pid_;
        std::string filename_;
        char state_;
        int ppid_;
        int pgrp_;
        int session_;
        int tty_nr_;
        int tpgid_;
        unsigned long flags_;
        unsigned long minflt_;
        unsigned long cminflt_;
        unsigned long majflt_;
        unsigned long cmajflt_;
        unsigned long utime_;
        unsigned long stime_;
        long cutime_;
        long cstime_;
        long priority_;
        long nice_;
        long num_threads_;
        long itrealvalue_;
        unsigned long starttime_;
        unsigned long vsize_;
        long rss_;
        unsigned long rlim_;
        unsigned long startcode_;
        unsigned long endcode_;
        unsigned long startstack_;
        unsigned long kstkesp_;
        unsigned long kstkeip_;
        unsigned long signal_;
        unsigned long blocked_;
        unsigned long sigignore_;
        unsigned long sigcatch_;
        unsigned long wchan_;
        unsigned long nswap_;
        unsigned long cnswap_;
        int exit_signal_;
        int processor_;
        unsigned long rt_priority_;
        unsigned long policy_;
        unsigned long long delayacct_blkio_ticks_;
        unsigned long guest_time_;
        long cguest_time_;

        // Since Linux 3.3
        unsigned long start_data_;
        unsigned long end_data_;
        unsigned long start_brk_;
        unsigned long arg_start_;
        unsigned long arg_end_;
        unsigned long env_start_;
        unsigned long env_end_;
        int exit_code_;

        void parseBUffer(std::ifstream &buffer);
    };

    class Memory
    {
    private:
        static double getPageFailtFromOS();
    public:
        static double numMajPageFaults();
    };

}


#endif //EDGECAFFE_MEMORY_H
