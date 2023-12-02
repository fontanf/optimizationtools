#include "optimizationtools/utils/logger.hpp"

using namespace optimizationtools;

namespace
{

std::string add_suffix(
        std::string file_path,
        const std::string& suffix)
{
    if (file_path != "") {
        for (int i = file_path.length() - 1; i >= 0; --i) {
            if (file_path[i] == '.') {
                file_path.insert(i, "_" + suffix);
                break;
            }
        }
    }
    return file_path;
}

}

Logger::Logger(
        const Logger& logger,
        const std::string& suffix):
    Logger(
            logger.to_stdout_,
            logger.to_stderr_,
            add_suffix(logger.log_path_, suffix),
            {}) { }
