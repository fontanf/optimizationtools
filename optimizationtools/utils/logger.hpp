#pragma once

#define FFOT_USE_JSON 1

#define FFOT_COMMA ,

#ifdef NDEBUG

#define FFOT_DBG(x)
#define FFOT_LOG(logger, message) {  }
#define FFOT_LOG_FOLD_START(logger, message) {  }
#define FFOT_LOG_FOLD_END(logger, message) {  }
#define FFOT_LOG_FOLD(logger, message) {  }
#define FFOT_LOG_ON(logger) {  }
#define FFOT_LOG_OFF(logger) {  }

#else

#define FFOT_DBG(x) x
#define FFOT_LOG(logger, message) \
    { \
        if (logger->enabled())  \
            logger->os() << message; \
    }
#define FFOT_LOG_FOLD_START(logger, message) \
    { \
        if (logger->enabled()) { \
            logger->increment_level(); \
            logger->os() << message; \
        } \
    }
#define FFOT_LOG_FOLD_END(logger, message) \
    { \
        if (logger->enabled()) { \
            logger->os() << message << " }}}" << std::endl; \
            logger->decrement_level(); \
        } \
    }
#define FFOT_LOG_FOLD(logger, message) \
    { \
        if (logger->enabled()) { \
            logger->increment_level(); \
            logger->os() << "{{{ " << message << " }}}" << std::endl; \
            logger->decrement_level(); \
        } \
    }
#define FFOT_LOG_ON(logger) { logger.enable(); }
#define FFOT_LOG_OFF(logger) { logger.disable(); }

#endif

#include "optimizationtools/utils/compose_stream.hpp"

namespace optimizationtools
{

/**
 * Logger structure
 */
class Logger
{

public:

    /*
     * Constructors and destructors
     */

    Logger(
            bool to_stdout,
            bool to_stderr,
            std::string file_path,
            const std::vector<std::ostream*>& ostreams):
        to_stdout_(to_stdout),
        to_stderr_(to_stderr),
        log_path_(file_path),
        os_(to_stdout, to_stderr, file_path, ostreams) { }

    Logger(
            const Logger& logger,
            const std::string& suffix);

    /*
     * Setters
     */

    /** Enable logs. */
    void enable() { on_ = true; }

    /** Disable logs. */
    void disable() { on_ = false; }

    /** Get output stream. */
    std::ostream& os() { return os_; }

    /*
     * Getters
     */

    bool enabled() const { return on_; }

private:

    /** Enable logging. */
    bool on_ = true;

    /** Write to stdout. */
    bool to_stdout_ = false;

    /** Write to stderr. */
    bool to_stderr_ = false;

    /** Log path. */
    std::string log_path_;

    /** ostreams. */
    ComposeStream os_;

};

}
