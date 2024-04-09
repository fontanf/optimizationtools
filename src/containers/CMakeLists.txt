add_library(optimizationtools_containers INTERFACE)
target_include_directories(optimizationtools_containers INTERFACE
    ${PROJECT_SOURCE_DIR}/include)
add_library(OptimizationTools::containers ALIAS optimizationtools_containers)
