add_library(OptimizationTools_utils)
target_sources(OptimizationTools_utils PRIVATE
    timer.cpp
    compose_stream.cpp
    logger.cpp
    parameters.cpp
    output.cpp)
target_include_directories(OptimizationTools_utils PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(OptimizationTools_utils PUBLIC
    nlohmann_json::nlohmann_json)
add_library(OptimizationTools::utils ALIAS OptimizationTools_utils)
set_target_properties(OptimizationTools_utils PROPERTIES OUTPUT_NAME "optimizationtools_utils")
install(TARGETS OptimizationTools_utils)
