cc_library(
        name = "graph",
        hdrs = [
                "abstract_graph.hpp",
                "adjacency_list_graph.hpp",
                "adjacency_matrix_graph.hpp",
                "clique_graph.hpp",
        ],
        srcs = [
                "adjacency_list_graph.cpp",
                "clique_graph.cpp",
        ],
        deps = [
                "//optimizationtools/utils:utils",
                "//optimizationtools/containers:containers",
        ],
        visibility = ["//visibility:public"],
)

