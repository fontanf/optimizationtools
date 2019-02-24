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

#ifdef NDEBUG

#define DBG(x)
#define LOG(info, message) {  }
#define LOG_FOLD_START(info, message) {  }
#define LOG_FOLD_END(info, message) {  }
#define LOG_FOLD(info, message) {  }
#define LOG_ON(info) {  }
#define LOG_OFF(info) {  }

#else

#define DBG(x) x
#define LOG(info, message) \
    { \
        if (info.logger.on && info.logger.level <= info.logger.level_max) { \
            if (info.logger.log_file.is_open()) \
                info.logger.log_file << message; \
            if (info.logger.log2stderr) \
                std::cerr << message; \
        } \
    }
#define LOG_FOLD_START(info, message) \
    { \
        info.logger.level++; \
        LOG(info, "{{{ " << message); \
    }
#define LOG_FOLD_END(info, message) \
    { \
        LOG(info, message << " }}}" << std::endl); \
        info.logger.level--; \
    }
#define LOG_FOLD(info, message) \
    { \
        info.logger.level++; \
        LOG(info, "{{{ " << message << " }}}" << std::endl); \
        info.logger.level--; \
    }
#define LOG_ON(info)  { info.logger.on = true; }
#define LOG_OFF(info) { info.logger.on = false; }

#endif

struct Logger
{
    Logger(std::string filepath = "", bool log2stderr = false, int level_max=999):
        log2stderr(log2stderr), level_max(level_max)
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
    bool on = true;
    bool log2stderr = false;
    std::ofstream log_file;
    int level = 0;
    int level_max = 999;
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

