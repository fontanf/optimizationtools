#pragma once

#define FFOT_USE_JSON 1

#define FFOT_TOL 0.000001

#include "optimizationtools/utils/timer.hpp"
#include "optimizationtools/utils/logger.hpp"

#if FFOT_USE_JSON == 1
#include <nlohmann/json.hpp>
#endif

#include <string>
#include <iostream>

namespace optimizationtools
{

/**
 * Structure that store the parameters of an optimization algorithm.
 */
struct Parameters
{
    /** Timer. */
    Timer timer;

    /*
     * Messages
     */

    /** Verbose level. */
    int verbosity_level = 1;

    /** Write messages to stdout. */
    bool messages_to_stdout = true;

    /** Path to the messages file. */
    std::string messages_path = "";

    /** Output streams. */
    std::vector<std::ostream*> messages_streams;

    /*
     * Logs
     */

    /** Logger, if provided. */
    std::shared_ptr<Logger> logger = nullptr;

    /** Path to the log file. */
    std::string log_path = "";

    /** Write logs to stderr. */
    bool log_to_stderr = false;


    /** Create the output stream. */
    std::unique_ptr<ComposeStream> create_os() const;

    /**
     * Get the logger.
     *
     * If the "logger" attribute is not "nullptr", then it is returned.
     * Otherwise, a new logger is created and set according to the other logger
     * parameters.
     */
    std::shared_ptr<Logger> get_logger() const;

    virtual int format_width() const { return 22; }

    /** Write a formatted output of the object to a stream. */
    virtual void format(
            std::ostream& os) const;

#if FFOT_USE_JSON == 1
    /** Write a formatted output of the object to a stream. */
    virtual nlohmann::json to_json() const;
#endif
};

}
