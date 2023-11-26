/**
 * This package provides a structure to pass as argument of optimization
 * algorithms which simplifies several aspects of the implementation.
 *
 * It includes an "Ouptut" object and a "Logger" object.
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
 *     info.output().set_verbosity_leve(1);
 * - Write something on the standard output:
 *     info.output() << "Print " << message;
 *
 * Certificate path features:
 * - Set the path of the certificate file:
 *     info.output().set_certificate_path("certificate_path");
 *
 * JSON output features:
 * - The JSON features rely on the  nlohmann/json package.
 * - They can be disabled by setting 'FFOT_USE_JSON' to '0', and thus, avoid
 *   the dependency.
 * - Set the path of the JSON output file:
 *     info.output().add_to_json_output_path("json_output_path");
 * - Write something in the JSON output file (thread safe):
 *     info.output().add_to_json("category", "key", value);
 *
 * Logging features:
 * - When compiling with NODEBUG, logging is disabled and no line related to
 *   the logging is kept in the final executable.
 * - Set the path of the log file:
 *     info.logger().set_log_path("log_path");
 * - Also write the log to the standard error output:
 *     info.logger().set_log_to_stderr(true);
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
#define FFOT_LOG(info, message) { info.logger << message; }
#define FFOT_LOG_FOLD_START(info, message) \
    { \
        info.logger().increment_level(); \
        FFOT_LOG(info, "{{{ " << message); \
    }
#define FFOT_LOG_FOLD_END(info, message) \
    { \
        FFOT_LOG(info, message << " }}}" << std::endl); \
        info.logger().decrement_level(); \
    }
#define FFOT_LOG_FOLD(info, message) \
    { \
        info.logger().increment_level(); \
        FFOT_LOG(info, "{{{ " << message << " }}}" << std::endl); \
        info.logger().decrement_level(); \
    }
#define FFOT_LOG_ON(info) { info.logger().enable(); }
#define FFOT_LOG_OFF(info) { info.logger().disable(); }

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

/**
 * Logger structure.
 */
class Logger
{

public:

    /*
     * Constructors and destructors
     */

    Logger() { }

    Logger(
            const Logger& logger,
            const std::string& suffix);

    /*
     * Setters
     */

    /** Enable logs to standard error output. */
    Logger& set_log_to_stderr(bool log_to_stderr) { log_to_stderr_ = log_to_stderr; return *this; }

    /** Set a maximum level for the logs. */
    Logger& set_maximum_log_level(int maximum_log_level) { maximum_log_level_ = maximum_log_level; return *this; }

    /** Set the path of the log file. */
    Logger& set_log_path(const std::string& log_path);

    /** Enable logs. */
    void enablel() { on_ = true; }

    /** Disable logs. */
    void disable() { on_ = false; }

    /** Increment debug log level. */
    void increment_level() { level_++; }

    /** Decrement debug log level. */
    void decrement_level() { level_--; }

    /** Write a log message. */
    template <typename T>
    Logger& operator<<(T& message)
    {
        if (on_ && level_ <= maximum_log_level_) {
            if (log_to_stderr_)
                std::cout << message;
            if (log_file_.is_open())
                std::cout << message;
            for (std::ofstream* os: ofstreams_)
                *os << message;
        }
        return *this;
    }

    Logger& operator<<(
            std::basic_ostream<std::ostream::char_type, std::ostream::traits_type>& (*func)(std::basic_ostream<std::ostream::char_type, std::ostream::traits_type>&))
    {
        if (log_to_stderr_)
            std::cout << func;
        if (log_file_.is_open())
            std::cout << func;
        for (std::ofstream* os: ofstreams_)
            *os << func;
        return *this;
    }

private:

    /** Enable logging. */
    bool on_ = true;

    /** Enable writing logs to standard error output. */
    bool log_to_stderr_ = false;

    /** Path to the log file. */
    std::string log_path_;

    /** Log file. */
    std::ofstream log_file_;

    /** Pointers to ofstreams. */
    std::vector<std::ofstream*> ofstreams_;

    /** Current level of the logs. */
    int level_ = 0;

    /** Maximum level of the logs. */
    int maximum_log_level_ = 999;

};

/**
 * Output structure.
 *
 * It stores everything related to the certificate file and the the JSON output
 * file.
 */
class Output
{

public:

    /*
     * Setters
     */

    /** Enable verbosity. */
    Output& set_verbosity_level(int verbosity_level) { verbosity_level_ = verbosity_level; return *this; }

    /** Enable logs to standard output. */
    Output& set_log_to_stdout(bool log_to_stdout) { log_to_stdout_ = log_to_stdout; return *this; }

    /** Set the path of the log file. */
    Output& set_log_path(const std::string& log_path);

    /** Set JSON output path. */
    Output& set_json_output_path(const std::string& json_output_path) { json_output_path_ = json_output_path; return *this; }

    /** Set certificate path. */
    Output& set_certificate_path(const std::string& certificate_path) { certificate_path_ = certificate_path; return *this; }

    /** Only write the output files at the end of the algorithm. */
    Output& set_only_write_at_the_end(bool only_write_at_the_end) { only_write_at_the_end_ = only_write_at_the_end; return *this; }

    /*
     * Getters
     */

    /** Get verbosity level. */
    int verbosity_level() const { return verbosity_level_; }

    /** Get certificate path. */
    const std::string& certificate_path() const { return certificate_path_; }

    /** Get only write at the end. */
    bool only_write_at_the_end() const { return only_write_at_the_end_; }

    /** Increment and return the new number of solutions. */
    int next_number_of_solutions()
    {
        number_of_solutions_++;
        return number_of_solutions_;
    }

    /** Increment and return the new number of bounds. */
    int next_number_of_bounds()
    {
        number_of_bounds_++;
        return number_of_bounds_;
    }

    /** Write a log message. */
    template <typename T>
    Output& operator<<(T const& message)
    {
        if (log_to_stdout_)
            std::cout << message;
        if (log_file_.is_open())
            std::cout << message;
        for (std::ofstream* os: ofstreams_)
            *os << message;
        return *this;
    }

    Output& operator<<(
            std::basic_ostream<std::ostream::char_type, std::ostream::traits_type>& (*func)(std::basic_ostream<std::ostream::char_type, std::ostream::traits_type>&))
    {
        if (log_to_stdout_)
            std::cout << func;
        if (log_file_.is_open())
            std::cout << func;
        for (std::ofstream* os: ofstreams_)
            *os << func;
        return *this;
    }

    /** Lock mutex. */
    void lock() { mutex_.lock(); }

    /** Unlock mutex. */
    void unlock() { mutex_.unlock(); }

    /*
     * JSON output
     */

    template<typename T1, typename T2, typename T3>
    void add_to_json(T1 category, T2 key, T3 value)
    {
#if FFOT_USE_JSON == 1
        json_[category][key] = value;
#endif
    }

    /** Write the JSON output file. */
    void write_json_output(const std::string& json_output_path) const;

    /** Write the JSON output file. */
    void write_json_output() const { write_json_output(json_output_path_); }

private:

    /** Enable writing logs to standard output. */
    bool log_to_stdout_ = true;

    /** Path to the log file. */
    std::string log_path_;

    /** Log file. */
    std::ofstream log_file_;

    /** Pointers to ofstreams. */
    std::vector<std::ofstream*> ofstreams_;

#if FFOT_USE_JSON == 1
    /** JSON output file. */
    nlohmann::json json_;
#endif

    /** Verbose level. */
    int verbosity_level_ = 0;

    /** Only write outputs at the end of the algorithm. */
    bool only_write_at_the_end_ = true;

    /** Path to the JSON output file. */
    std::string json_output_path_  = "";

    /** Path to the certificate file. */
    std::string certificate_path_ = "";

    /** Mutex. */
    std::mutex mutex_;

    /** Counter for the number of solutions. */
    int number_of_solutions_ = 0;

    /** Counter for the number of bounds. */
    int number_of_bounds_ = 0;

};

class Info
{

public:

    /*
     * Constructors and destructors
     */

    /** Constructor. */
    Info();

    /**
     * Copy an existing Info object.
     *
     * This is meant to be used
     * - in parallel algorithms: in this case, the Output object should be kept
     *   and a new Logger object should be used.
     * - When calling another sub-algorithm inside the algorithm: in this case,
     *   the Logger object should be kept and a new Output object should be
     *   used.
     *
     * Keep the Output object iff 'keep_output == true'.
     *
     * Keep the Logger object iff 'logger_suffix == ""'.  Otherwise, create a
     * new Logger structure with 'logger_suffix' appended to the log path
     * (before the extension).
     *
     * Note that the time limit and the sigint handler are always kept.
     */
    Info(
            const Info& info,
            bool keep_output,
            std::string logger_suffix);

    /*
     * Setters
     */

    /** Set the time limit of the algorithm. */
    Info& set_time_limit(double time_limit) { time_limit_ = time_limit; return *this; }

    /** Set SIGINT handler. */
    Info& set_sigint_handler();

    /*
     * Getters
     */

    /** Get the logger object. */
    Logger& logger() { return *logger_; }

    /** Get the output object. */
    Output& output() { return *output_; }

    /*
     * Stop criteria
     */

    /** Get the elapsed time since the start of the algorithm. */
    double elapsed_time() const;

    /** Get the remaining time before reaching the time limit. */
    double remaining_time() const { return std::max(0.0, time_limit_ - elapsed_time()); }

    /** Return 'true' iff the time limit has not been reached yet. */
    bool check_time() const { return elapsed_time() <= time_limit_; }

    /** Reset the starting time of the algorithm. */
    void reset_time() { start_ = std::chrono::high_resolution_clock::now(); }

    /** Return 'true' iff the program has received the SIGINT signal. */
    bool terminated_by_sigint() const;

    /** Return 'true' iff the algorithm needs to end. */
    bool needs_to_end() const
    {
        return !check_time()
            || terminated_by_sigint()
            || (end_ != nullptr && *end_);
    }

private:

    /*
     * Private attributes
     */

    /** Pointer to the logger. */
    std::shared_ptr<Logger> logger_ = NULL;

    /** Pointer to the output structure. */
    std::shared_ptr<Output> output_ = NULL;

    /** Start time of the algorithm. */
    std::chrono::high_resolution_clock::time_point start_;

    /** Time limit of the algorithm. */
    double time_limit_ = std::numeric_limits<double>::infinity();

    /** Flag that the user can set to 'true' to tell the algorithm to stop. */
    bool* end_ = nullptr;

};

}

