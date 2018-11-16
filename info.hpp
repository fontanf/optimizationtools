#pragma once

#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct Info
{
    Info(bool v = false, bool d = false):
        verbose(v),
        dbg(d),
        t1(std::chrono::high_resolution_clock::now()) { }

    ~Info() { }

    boost::property_tree::ptree pt;
    bool verbose;
    bool dbg;
    std::string debug_string;
    std::ofstream dout;
    std::chrono::high_resolution_clock::time_point t1;

    void debug(std::string message)
    {
        if (dbg)
            debug_string += message;
    }

    void write_ini(std::string file)
    {
        if (file != "")
            boost::property_tree::write_ini(file, pt);
    }

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point t2
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        return time_span.count();
    }
};

