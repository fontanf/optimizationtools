#include "benchtools/tools.hpp"

#include <sstream>
#include <iterator>
#include <vector>

using namespace benchtools;

Algorithm::Algorithm(std::string str)
{
    std::stringstream ss(str);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    name = vstrings[0];
    for (auto it = std::next(vstrings.begin());
            it != vstrings.end() && std::next(it)!=vstrings.end();
            it = std::next(std::next(it)))
        args[*it] = *std::next(it);
}

bool benchtools::replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

