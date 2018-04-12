#pragma once

#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct Info
{
    Info(): t1(std::chrono::high_resolution_clock::now()) {  }
    boost::property_tree::ptree pt;
    bool verbose_ = false;
    std::chrono::high_resolution_clock::time_point t1;

    void verbose(bool b) { verbose_ = b; }
    static bool verbose(const Info* info)
    {
        return (info != NULL && info->verbose_);
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

