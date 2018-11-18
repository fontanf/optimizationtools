#pragma once

#include <chrono>
#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct Info
{
    Info(): t1(std::chrono::high_resolution_clock::now()) { }

    ~Info() { }

    boost::property_tree::ptree pt;
    bool verbose_ = false;
    bool dbg_ = false;
    bool dbg_live_ = false;
    std::string debug_string;
    std::chrono::high_resolution_clock::time_point t1;

    void set_verbose() { verbose_ = true; }
    void set_debug() { dbg_ = true; }
    void set_debuglive() { dbg_live_ = true; }
    bool verbose() { return verbose_; }
    bool debug() { return (dbg_ || dbg_live_); }
    void debug(std::string message)
    {
        if (dbg_)
            debug_string += message;
        if (dbg_live_)
            std::cout << message;
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

