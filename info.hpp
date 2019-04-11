#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Logger is not thread safe, each thread should have its own Logger.
 * Output is thread safe, it should be shared between threads.
 */

#define VER(info, message) \
    if (info.output->verbose) { \
        info.output->mutex_cout.lock(); \
        std::cout << message; \
        info.output->mutex_cout.unlock(); \
    }

#define PUT(info, key, value) \
    info.output->mutex_pt.lock(); \
    info.output->pt.put(key, value); \
    info.output->mutex_pt.unlock();

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
        if (info.logger->on && info.logger->level <= info.logger->level_max) { \
            if (info.logger->logfile.is_open()) \
                info.logger->logfile << message; \
            if (info.logger->log2stderr) \
                std::cerr << message; \
        } \
    }
#define LOG_FOLD_START(info, message) \
    { \
        info.logger->level++; \
        LOG(info, "{{{ " << message); \
    }
#define LOG_FOLD_END(info, message) \
    { \
        LOG(info, message << " }}}" << std::endl); \
        info.logger->level--; \
    }
#define LOG_FOLD(info, message) \
    { \
        info.logger->level++; \
        LOG(info, "{{{ " << message << " }}}" << std::endl); \
        info.logger->level--; \
    }
#define LOG_ON(info)  { info.logger->on = true; }
#define LOG_OFF(info) { info.logger->on = false; }

#endif


struct Logger
{
    bool on = true;
    bool log2stderr = false;
    std::ofstream logfile;
    std::string logfilename;
    int level = 0;
    int level_max = 999;
};

struct Output
{
    boost::property_tree::ptree pt;
    bool onlywriteattheend = true;
    std::string inifile  = "";
    std::string certfile = "";
    std::mutex mutex_pt;
    std::mutex mutex_cout;
    std::mutex mutex_sol;
    bool verbose = false;
    int sol_number = 0;
};

struct Info
{

public:

    Info()
    {
        logger = std::shared_ptr<Logger>(new Logger());
        output = std::shared_ptr<Output>(new Output());
        start = std::chrono::high_resolution_clock::now();
    }

    Info& set_verbose(bool verbose) { output->verbose = verbose; return *this; }
    Info& set_outputfile(std::string outputfile) { output->inifile = outputfile; return *this; }
    Info& set_certfile(std::string certfile) { output->certfile = certfile; return *this; }
    Info& set_onlywriteattheend(bool b) { output->onlywriteattheend = b; return *this; }
    Info& set_log2stderr(bool log2stderr) { logger->log2stderr = log2stderr; return *this; }
    Info& set_loglevelmax(int loglevelmax) { logger->level_max = loglevelmax; return *this; }
    Info& set_timelimit(double t) { timelimit = t; return *this; }
    Info& set_logfile(std::string logfile)
    {
        if (logfile == "")
            return *this;
        if (logger->logfile.is_open())
            logger->logfile.close();
        logger->logfile.open(logfile);
        return *this;
    }

    Info(const Info& info, bool keep_output, std::string keep_logger)
    {
        if (keep_logger == "") {
            logger = std::shared_ptr<Logger>(info.logger);
        } else {
            std::string logfile = info.logger->logfilename;
            if (logfile != "") {
                for (int i=logfile.length()-1; i>=0; --i) {
                    if (logfile[i] == '.') {
                        logfile.insert(i, "_" + keep_logger);
                        break;
                    }
                }
            }
            logger = std::shared_ptr<Logger>();
            set_logfile(logfile);
        }
        output = (!keep_output)? std::shared_ptr<Output>(new Output()):
                                 std::shared_ptr<Output>(info.output);
        start = info.start;
        timelimit = info.timelimit;
    }

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point end
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return time_span.count();
    }

    bool check_time() const { return elapsed_time() <= timelimit; }

    void write_ini() const { write_ini(output->inifile); }
    void write_ini(std::string filename) const
    {
        if (filename != "")
            boost::property_tree::write_ini(filename, output->pt);
    }


    std::shared_ptr<Logger> logger = NULL;
    std::shared_ptr<Output> output = NULL;
    std::chrono::high_resolution_clock::time_point start;
    double timelimit = std::numeric_limits<double>::infinity();
};

