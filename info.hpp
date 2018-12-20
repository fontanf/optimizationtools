#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define PRINT(x) "x " << x

#define VER(info, message) \
    if (info.verbose) \
        std::cout << message;

#define PUT(info, key, value) \
    info.pt.put(key, value);

#define LOG_FOLD_START "{{{"
#define LOG_FOLD_END   "}}}"

#ifdef NDEBUG

#define LOG(info, message) {  }

#else

#define LOG(info, message) \
    { \
        if (info.logger.log_file.is_open()) \
            info.logger.log_file << message; \
        if (info.logger.log2stderr) \
            std::cerr << message; \
    }

#endif

struct Logger
{
    Logger(std::string filepath = "", bool log2stderr = false): log2stderr(log2stderr)
    {
        if (filepath != "")
            log_file.open(filepath);
    }

    ~Logger()
    {
        if (log_file.is_open())
            log_file.close();
    }

    // Logger
    bool log2stderr = false;
    std::ofstream log_file;
};

struct Info
{
    Info(Logger& logger, bool verbose = false):
        logger(logger),
        verbose(verbose),
        t1(std::chrono::high_resolution_clock::now()) { }

    Logger& logger;

    bool verbose = false;

    /**
     * Time
     */

    std::chrono::high_resolution_clock::time_point t1;

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point t2
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        return time_span.count();
    }

    /**
     * Info
     */

    boost::property_tree::ptree pt;
    bool only_write_at_the_end = true;
    std::string ini_file  = "";
    std::string cert_file  = "";

    void write_ini() const { write_ini(ini_file); }
    void write_ini(std::string filename) const
    {
        if (filename != "")
            boost::property_tree::write_ini(filename, pt);
    }

};

