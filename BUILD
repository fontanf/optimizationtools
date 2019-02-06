cc_library(
        name = "info",
        hdrs = ["info.hpp"],
        srcs = [],
        visibility = ["//visibility:public"],
        linkopts = [
                "-lboost_system",
                "-lboost_filesystem",
        ],
        deps = [
        ],
        include_prefix = "benchtools/",
)

cc_binary(
        name = "bench",
        srcs = ["bench.cpp"],
        copts = ["-pthread"],
        linkopts = [
                "-lpthread",
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options",
                "-lboost_timer"
        ],
        deps = [
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "timedbench",
        srcs = ["timedbench.cpp"],
        copts = ["-pthread"],
        linkopts = [
                "-lpthread",
                "-lboost_system",
                "-lboost_filesystem",
                "-lboost_program_options",
        ],
        deps = [
        ],
        visibility = ["//visibility:public"],
)
