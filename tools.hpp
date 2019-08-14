#pragma once

#include <map>

namespace benchtools
{

struct Algorithm
{
    /**
     * Input
     * * str: "algoname param1 value1 param2 valu2"
     * Output
     * * name: "algoname"
     * * args: {"param1": "value1", "param2": "value2"}
     */
    Algorithm(std::string str);
    std::string name;
    std::map<std::string, std::string> args;
};
Algorithm read_algorirthm_string(std::string str);

bool replace(std::string& str, const std::string& from, const std::string& to);

}

