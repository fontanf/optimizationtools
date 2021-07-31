#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <iomanip>
#include <memory>

#include <nlohmann/json.hpp>

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

#define PUT(info, cat, key, value) \
    info.output->mutex_json.lock(); \
    info.output->j[cat][key] = value; \
    info.output->mutex_json.unlock();

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
        if (info.logger->on && info.logger->level <= info.logger->maximum_log_level) { \
            if (info.logger->log_file.is_open()) \
                info.logger->log_file << message; \
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

#define TOL 0.000001

namespace optimizationtools
{

struct Logger
{
    bool on = true;
    bool log2stderr = false;
    std::ofstream log_file;
    std::string log_path;
    int level = 0;
    int maximum_log_level = 999;
};

struct Output
{
    nlohmann::json j;
    bool only_write_at_the_end = true;
    std::string json_output_path  = "";
    std::string certificate_path = "";
    std::mutex mutex_json;
    std::mutex mutex_cout;
    std::mutex mutex_solutions;
    bool verbose = false;
    int number_of_solutions = 0;
    int number_of_bounds = 0;
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
    Info& set_json_output_path(std::string outputfile) { output->json_output_path = outputfile; return *this; }
    Info& set_certificate_path(std::string certfile) { output->certificate_path = certfile; return *this; }
    Info& set_only_write_at_the_end(bool b) { output->only_write_at_the_end = b; return *this; }
    Info& set_log2stderr(bool log2stderr) { logger->log2stderr = log2stderr; return *this; }
    Info& set_maximum_log_level(int maximum_log_level) { logger->maximum_log_level = maximum_log_level; return *this; }
    Info& set_time_limit(double t) { time_limit = t; return *this; }
    Info& set_log_path(std::string log_path)
    {
        logger->log_path = log_path;
        if (log_path == "")
            return *this;
        if (logger->log_file.is_open())
            logger->log_file.close();
        logger->log_file.open(log_path);
        return *this;
    }

    Info(const Info& info, bool keep_output, std::string keep_logger)
    {
        if (keep_logger == "") {
            logger = std::shared_ptr<Logger>(info.logger);
        } else {
            std::string log_file = info.logger->log_path;
            if (log_file != "") {
                for (int i=log_file.length()-1; i>=0; --i) {
                    if (log_file[i] == '.') {
                        log_file.insert(i, "_" + keep_logger);
                        break;
                    }
                }
            }
            logger = std::shared_ptr<Logger>(new Logger());
            set_log_path(log_file);
        }
        output = (!keep_output)? std::shared_ptr<Output>(new Output()):
                                 std::shared_ptr<Output>(info.output);
        start = info.start;
        time_limit = info.time_limit;
    }

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point end
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return time_span.count();
    }

    double remaining_time() const { return std::max(0.0, time_limit - elapsed_time()); }
    bool check_time() const { return elapsed_time() <= time_limit; }
    void reset_time() { start = std::chrono::high_resolution_clock::now(); }

    void write_json_output() const { write_json_output(output->json_output_path); }
    void write_json_output(std::string filename) const
    {
        if (filename != "") {
            output->mutex_json.lock();
            std::ofstream o(filename);
            o << std::setw(4) << output->j << std::endl;
            output->mutex_json.unlock();
        }
    }

    /**
     * Attributes
     */

    std::shared_ptr<Logger> logger = NULL;
    std::shared_ptr<Output> output = NULL;
    std::chrono::high_resolution_clock::time_point start;
    double time_limit = std::numeric_limits<double>::infinity();
};

}

