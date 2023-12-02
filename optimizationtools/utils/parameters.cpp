#include "optimizationtools/utils/parameters.hpp"

#include <iomanip>

using namespace optimizationtools;

std::unique_ptr<ComposeStream> Parameters::create_os() const
{
    return std::unique_ptr<ComposeStream>(new ComposeStream(
                messages_to_stdout,
                false,
                messages_path,
                messages_streams));
}

std::shared_ptr<Logger> Parameters::get_logger() const
{
    if (logger != nullptr) {
        return logger;
    } else {
        return std::shared_ptr<Logger>(new Logger(
                    false,
                    log_to_stderr,
                    log_path,
                    {}));
    }
}

nlohmann::json Parameters::to_json() const
{
    return nlohmann::json {
        {"TimeLimit", timer.time_limit()},
        {"VerbosityLevel", verbosity_level},
        {"Messages",
            {"VerbosityLevel", verbosity_level},
            {"StandardOutput", messages_to_stdout},
            {"FilePath", messages_path},
            {"NumberOfStreams", messages_streams.size()}},
        {"Logger",
            {"HasLogger", (logger != nullptr)},
            {"StardardError", log_to_stderr},
            {"FilePath", log_path}}};
}

void Parameters::format(
        std::ostream& os) const
{
    int width = format_width();
    os
        << std::setw(width) << std::left << "Time limit: " << timer.time_limit() << std::endl
        << "Messages" << std::endl
        << std::setw(width) << std::left << "    Verbosity level: " << verbosity_level << std::endl
        << std::setw(width) << std::left << "    Standard output: " << messages_to_stdout << std::endl
        << std::setw(width) << std::left << "    File path: " << messages_path << std::endl
        << std::setw(width) << std::left << "    # streams: " << messages_streams.size() << std::endl
        << "Logger" << std::endl
        << std::setw(width) << std::left << "    Has logger: " << (logger != nullptr) << std::endl
        << std::setw(width) << std::left << "    Standard error: " << log_to_stderr << std::endl
        << std::setw(width) << std::left << "    File path: " << log_path << std::endl
        ;
}
