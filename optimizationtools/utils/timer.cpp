#include "optimizationtools/utils/timer.hpp"

#include <signal.h>
#include <iostream>

using namespace optimizationtools;

namespace
{

volatile sig_atomic_t sigint_flag = 0;

void sigint_handler(int)
{
    if (sigint_flag == 1) {
        std::cout << std::endl;
        exit(1);
    }
    sigint_flag = 1; // set flag
}

}

Timer::Timer():
    start_(std::chrono::high_resolution_clock::now())
{
}

Timer& Timer::set_sigint_handler()
{
    signal(SIGINT, sigint_handler);
    return *this;
}

double Timer::elapsed_time() const
{
    std::chrono::high_resolution_clock::time_point end
        = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span
        = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_);
    return time_span.count();
}

bool Timer::terminated_by_sigint() const
{
    return sigint_flag;
}
