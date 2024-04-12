#pragma once

#include "optimizationtools/utils/parameters.hpp"

#include <iostream>
#include <fstream>

#if FFOT_USE_JSON == 1
#include <nlohmann/json.hpp>
#endif

namespace optimizationtools
{

/**
 * Structure that store the output of an optimization algorithm.
 */
struct Output
{
#if FFOT_USE_JSON == 1
    /** JSON output file. */
    nlohmann::json json;
#endif


    /** Write JSON output to a file. */
    void write_json_output(const std::string& json_output_path) const;
};

}
