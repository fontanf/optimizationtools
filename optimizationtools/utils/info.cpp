#include "optimizationtools/utils/info.hpp"

#include <chrono>
#include <signal.h>

using namespace optimizationtools;

namespace
{

volatile sig_atomic_t sigint_flag = 0;

void sigint_handler(int)
{
    if (sigint_flag == 1) {
        std::cout << std::endl;
        exit(1);
    }
    sigint_flag = 1; // set flag
}

}

Logger::Logger(
        const Logger& logger,
        const std::string& suffix)
{
    std::string log_path = logger.log_path_;
    if (log_path != "") {
        for (int i = log_path.length() - 1; i >= 0; --i) {
            if (log_path[i] == '.') {
                log_path.insert(i, "_" + suffix);
                break;
            }
        }
    }
    set_log_path(log_path);
}

Logger& Logger::set_log_path(const std::string& log_path)
{
    log_path_ = log_path;
    if (log_file_.is_open())
        log_file_.close();
    if (log_path_ == "")
        return *this;
    log_file_.open(log_path);
    return *this;
}

Output& Output::set_log_path(const std::string& log_path)
{
    log_path_ = log_path;
    if (log_file_.is_open())
        log_file_.close();
    if (log_path_ == "")
        return *this;
    log_file_.open(log_path);
    return *this;
}

void Output::write_json_output(const std::string& json_output_path) const
{
#if FFOT_USE_JSON == 1
    if (json_output_path == "")
        return;
    std::ofstream file(json_output_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + json_output_path + "\".");
    }
    file << std::setw(4) << json_ << std::endl;
#endif
}

Info::Info():
    logger_(std::shared_ptr<Logger>(new Logger())),
    output_(std::shared_ptr<Output>(new Output())),
    start_(std::chrono::high_resolution_clock::now())
{
}

Info::Info(
        const Info& info,
        bool keep_output,
        std::string logger_suffix)
{
    if (logger_suffix == "") {
        logger_ = std::shared_ptr<Logger>(info.logger_);
    } else {
        logger_ = std::shared_ptr<Logger>(new Logger(*info.logger_, logger_suffix));
    }

    if (keep_output) {
        output_ = std::shared_ptr<Output>(info.output_);
    } else {
        output_ = std::shared_ptr<Output>(new Output());
    }

    start_ = info.start_;
    time_limit_ = info.time_limit_;
    end_ = info.end_;
}

Info& Info::set_sigint_handler()
{
    signal(SIGINT, sigint_handler);
    return *this;
}

double Info::elapsed_time() const
{
    std::chrono::high_resolution_clock::time_point end
        = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span
        = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_);
    return time_span.count();
}

bool Info::terminated_by_sigint() const
{
    return sigint_flag;
}
