add_library(OptimizationTools_containers INTERFACE)
target_include_directories(OptimizationTools_containers INTERFACE
    ${PROJECT_SOURCE_DIR}/include)
add_library(OptimizationTools::containers ALIAS OptimizationTools_containers)
