#pragma once

#if defined(__GNUC__) || defined(__clang__)
#  define FUNC_SIGNATURE std::string(__PRETTY_FUNCTION__)
#elif defined(_MSC_VER)
#  define FUNC_SIGNATURE std::string(__FUNCSIG__)
#else
#  define FUNC_SIGNATURE std::string(__func__)
#endif
