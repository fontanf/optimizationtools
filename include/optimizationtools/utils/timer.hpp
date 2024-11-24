#pragma once

#include <chrono>
#include <algorithm>
#include <vector>

namespace optimizationtools
{

/**
 * Timer class
 */
class Timer
{

public:

    /*
     * Constructors and desctructor
     */

    /** Constructor. */
    Timer();

    /*
     * Setters
     */

    /** Set the time limit of the algorithm. */
    Timer& set_time_limit(double time_limit) { time_limit_ = time_limit; return *this; }

    /** Add an end boolean. */
    Timer& add_end_boolean(const bool* end) { end_.push_back(end); return *this; }

    /** Set SIGINT handler. */
    Timer& set_sigint_handler();

    /*
     * Getters
     */

    /*
     * Stop criteria
     */

    /** Get the time limit. */
    double time_limit() const { return time_limit_; }

    /** Get the elapsed time since the start of the algorithm. */
    double elapsed_time() const;

    /** Get the remaining time before reaching the time limit. */
    double remaining_time() const { return (std::max)(0.0, time_limit_ - elapsed_time()); }

    /** Return 'true' iff the time limit has not been reached yet. */
    bool check_time() const { return elapsed_time() <= time_limit_; }

    /** Reset the starting time of the algorithm. */
    void reset_time() { start_ = std::chrono::high_resolution_clock::now(); }

    /** Return 'true' iff the program has received the SIGINT signal. */
    bool terminated_by_sigint() const;

    /** Return 'true' iff the algorithm needs to end. */
    bool needs_to_end() const
    {
        for (const bool* end: end_)
            if (*end)
                return true;
        return !check_time()
            || terminated_by_sigint();
    }

private:

    /** Start time of the algorithm. */
    std::chrono::high_resolution_clock::time_point start_;

    /** Time limit of the algorithm. */
    double time_limit_ = std::numeric_limits<double>::infinity();

    /** Flag that the user can set to 'true' to tell the algorithm to stop. */
    std::vector<const bool*> end_;

};

}
