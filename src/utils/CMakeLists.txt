add_library(optimizationtools_utils)
target_sources(optimizationtools_utils PRIVATE
    timer.cpp
    compose_stream.cpp
    logger.cpp
    parameters.cpp
    output.cpp)
target_include_directories(optimizationtools_utils PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(optimizationtools_utils PUBLIC
    nlohmann_json::nlohmann_json)
add_library(OptimizationTools::utils ALIAS optimizationtools_utils)
