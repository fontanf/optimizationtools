#include "optimizationtools/utils/output.hpp"

#include <iomanip>

using namespace optimizationtools;

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
    file << std::setw(4) << json << std::endl;
#endif
}
