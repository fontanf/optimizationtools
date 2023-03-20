/**
 * This package provides a structure to pass as argument of optimization
 * algorithms which simplifies several aspects of the implementation.
 *
 * It is designed such that the user can use a default value:
 *     algorithm(Instance instance, Info info = Info());
 *
 * Thus, this works:
 *     algorithm(instance);
 * as well as:
 *     Info info = Info().set_time_limit(60);
 *     algorithm(instance, info);
 *
 * Verbosity features:
 * - Enable verbosity:
 *     info.set_verbosity_leve(1);
 * - Write something on the standard output:
 *     info.os() << "Print " << message;
 *
 * Certificate path features:
 * - Set the path of the certificate file:
 *     info.set_certificate_path("certificate_path");
 *
 * JSON output features:
 * - The JSON features rely on the  nlohmann/json package.
 * - They can be disabled by setting 'FFOT_USE_JSON' to '0', and thus, avoid
 *   the dependency.
 * - Set the path of the JSON output file:
 *     info.add_to_json_output_path("json_output_path");
 * - Write something in the JSON output file (thread safe):
 *     info.add_to_json("category", "key", value);
 *
 * Logging features:
 * - When compiling with NODEBUG, logging is disabled and no line related to
 *   the logging is kept in the final executable.
 * - Set the path of the log file:
 *     info.set_log_path("log_path");
 * - Also write the log to the standard error output:
 *     info.set_log2stderr(true);
 * - Disable/enable logging (enabled by default):
 *     FFOT_LOG_OFF(info);
 *     FFOT_LOG_ON(info);
 * - Write some logging information:
 *     FFOT_LOG(info, "I want to log " << message << std::endl);
 * - Start a new fold:
 *     FFOT_LOG_FOLD_START(info, "new fold" << std::endl);
 * - End the current fold:
 *     FFOT_LOG_FOLD_END(info, "new fold");
 *
 * Time limit features:
 * - Set the time limit (in seconds) of the algorithm:
 *     info.set_time_limit(60);
 * - Check if the time limit has been reached:
 *     info.check_time();
 * - Get the elapsed time:
 *     info.elapsed_time();
 * - Get the remaining time:
 *     info.remaining_time();
 * - Reset the starting time:
 *     info.reset_time();
 *
 * SIGINT handler:
 * - Enable SIGINT handler:
 *     info.set_sigint_handler()
 * - Check if the program has received SIGINT signal:
 *     info.terminated_by_sigint()
 * - Check SIGINT and time limit:
 *     info.needs_to_end()
 *
 * Example of function to update an incumbent solution:

void update_solution(
        Solution& incumbent_solution,
        const Solution& new_solution,
        optimizationtools::Info& info)
{
    info.lock();

    if (incumbent_solution is worse than new_solution) {
        incumbent_solution = new_solution;

        info.output->number_of_solutions++;
        double t = round(info.elapsed_time() * 10000) / 10000;
        info.os()
                << "Time: " << t
                << "; New solution with value: " << incumbent_solution.value()
                << std::endl);
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Value", incumbent_solution.value());
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end) {
            info.write_json_output();
            solution.write(info.output->certificate_path);
        }
    }

    info.unlock();
}

*/

#pragma once

#define FFOT_USE_JSON 1

#define FFOT_COMMA ,

#ifdef NDEBUG

#define FFOT_DBG(x)
#define FFOT_LOG(info, message) {  }
#define FFOT_LOG_FOLD_START(info, message) {  }
#define FFOT_LOG_FOLD_END(info, message) {  }
#define FFOT_LOG_FOLD(info, message) {  }
#define FFOT_LOG_ON(info) {  }
#define FFOT_LOG_OFF(info) {  }

#else

#define FFOT_DBG(x) x
#define FFOT_LOG(info, message) \
    { \
        if (info.logger->on && info.logger->level <= info.logger->maximum_log_level) { \
            if (info.logger->log_file.is_open()) \
                info.logger->log_file << message; \
            if (info.logger->log2stderr) \
                std::cerr << message; \
        } \
    }
#define FFOT_LOG_FOLD_START(info, message) \
    { \
        info.logger->level++; \
        FFOT_LOG(info, "{{{ " << message); \
    }
#define FFOT_LOG_FOLD_END(info, message) \
    { \
        FFOT_LOG(info, message << " }}}" << std::endl); \
        info.logger->level--; \
    }
#define FFOT_LOG_FOLD(info, message) \
    { \
        info.logger->level++; \
        FFOT_LOG(info, "{{{ " << message << " }}}" << std::endl); \
        info.logger->level--; \
    }
#define FFOT_LOG_ON(info)  { info.logger->on = true; }
#define FFOT_LOG_OFF(info) { info.logger->on = false; }

#endif

#define FFOT_TOL 0.000001

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <iomanip>
#include <memory>

#if FFOT_USE_JSON == 1
#include <nlohmann/json.hpp>
#endif

namespace optimizationtools
{

class OutputStream: public std::ostream
{
    struct ComposeBuffer: public std::streambuf
    {
        void add_buffer(std::streambuf* buf)
        {
            bufs.push_back(buf);
        }

        virtual int overflow(int c)
        {
            for (auto buf: bufs)
                buf->sputc(c);
            return c;
        }

        std::vector<std::streambuf*> bufs;
    };

public:

    OutputStream():
        std::ostream(NULL)
    {
        std::ostream::rdbuf(&buffer);
    }

    void link_stream(std::ostream& out)
    {
        out.flush();
        buffer.add_buffer(out.rdbuf());
    }

private:

    ComposeBuffer buffer;

};

struct Info
{

public:

    /**
     * Logger structure.
     */
    struct Logger
    {
        /** Enable logging. */
        bool on = true;
        /** Enable writing logs to standard error output. */
        bool log2stderr = false;
        /** Log file. */
        std::ofstream log_file;
        /** Path to the log file. */
        std::string log_path;
        /** Current level of the logs. */
        int level = 0;
        /** Maximum level of the logs. */
        int maximum_log_level = 999;
    };

    /**
     * Output structure.
     *
     * It stores everything related to the certificate file and the the JSON output
     * file.
     */
    struct Output
    {
        /** Output stream. */
        OutputStream os;
#if FFOT_USE_JSON == 1
        /** JSON output file. */
        nlohmann::json json;
#endif
        /** Verbose level. */
        int verbosity_level = 0;
        /** Only write outputs at the end of the algorithm. */
        bool only_write_at_the_end = true;
        /** Path to the JSON output file. */
        std::string json_output_path  = "";
        /** Path to the certificate file. */
        std::string certificate_path = "";
        /** Mutex. */
        std::mutex mutex;
        /** Counter for the number of solutions. */
        int number_of_solutions = 0;
        /** Counter for the number of bounds. */
        int number_of_bounds = 0;
    };


    /*
     * Constructors and destructors.
     */

    /** Constructor. */
    Info();

    /**
     * Copy an existing Info structure.
     *
     * This is meant to be used in parallel algorithms.
     *
     * Keep the Output structure iff 'keep_output == true'.
     *
     * Keep the Logging structure iff 'keep_logger == ""'.  Otherwise, create a
     * new Logger structure with 'logger_string' append to the log path (before
     * the extension).
     *
     * Note that Info(info, false, "newthread") keeps the time limit.
     */
    Info(const Info& info, bool keep_output, std::string keep_logger);

    /*
     * Getters.
     */

    /** Get output stream. */
    OutputStream& os() { return output->os; }

    /** Get verbosity level. */
    int verbosity_level() const { return output->verbosity_level; }

    /*
     * Setters.
     */

    /** Enable verbosity. */
    Info& set_verbosity_level(int verbosity_level)
    {
        if (verbosity_level > 0) {
            output->os.clear();
        } else {
            output->os.setstate(std::ios_base::failbit);
        }
        output->verbosity_level = verbosity_level;
        return *this;
    }

    /** Set JSON output path. */
    Info& set_json_output_path(std::string outputfile) { output->json_output_path = outputfile; return *this; }

    /** Set certificate path. */
    Info& set_certificate_path(std::string certfile) { output->certificate_path = certfile; return *this; }

    /** Only write the output files at the end of the algorithm. */
    Info& set_only_write_at_the_end(bool b) { output->only_write_at_the_end = b; return *this; }

    /** Enable logs to standard error output. */
    Info& set_log2stderr(bool log2stderr) { logger->log2stderr = log2stderr; return *this; }

    /** Set a maximum level for the logs. */
    Info& set_maximum_log_level(int maximum_log_level) { logger->maximum_log_level = maximum_log_level; return *this; }

    /** Set the time limit of the algorithm. */
    Info& set_time_limit(double t) { time_limit = t; return *this; }

    /** Set the path of the log file. */
    Info& set_log_path(std::string log_path);

    /** Set SIGINT handler. */
    Info& set_sigint_handler();

    template<typename T1, typename T2, typename T3>
    void add_to_json(T1 category, T2 key, T3 value)
    {
#if FFOT_USE_JSON == 1
        output->json[category][key] = value;
#endif
    }

    /** Lock mutex. */
    void lock() { output->mutex.lock(); }

    /** Unlock mutex. */
    void unlock() { output->mutex.unlock(); }

    /*
     * Time.
     */

    /** Get the elapsed time since the start of the algorithm. */
    double elapsed_time() const;

    /** Get the remaining time before reaching the time limit. */
    double remaining_time() const { return std::max(0.0, time_limit - elapsed_time()); }

    /** Return 'true' iff the time limit has not been reached yet. */
    bool check_time() const { return elapsed_time() <= time_limit; }

    /** Reset the starting time of the algorithm. */
    void reset_time() { start = std::chrono::high_resolution_clock::now(); }

    /** Return 'true' iff the program has received the SIGINT signal. */
    bool terminated_by_sigint() const;

    /** Return 'true' iff the algorithm needs to end. */
    bool needs_to_end() const
    {
        return !check_time()
            || terminated_by_sigint()
            || (end != nullptr && *end);
    }

    /*
     * JSON output.
     */

    /** Write the JSON output file. */
    void write_json_output(std::string json_output_path) const;

    /** Write the JSON output file. */
    void write_json_output() const { write_json_output(output->json_output_path); }


    /*
     * Attributes
     */

    /** Pointer to the logger. */
    std::shared_ptr<Logger> logger = NULL;

    /** Pointer to the output structure. */
    std::shared_ptr<Output> output = NULL;

    /** Start time of the algorithm. */
    std::chrono::high_resolution_clock::time_point start;

    /** Time limit of the algorithm. */
    double time_limit = std::numeric_limits<double>::infinity();

    /** Flag that the user can set to 'true' to tell the algorithm to stop. */
    bool* end = nullptr;

};

}

