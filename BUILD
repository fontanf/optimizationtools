cc_binary(
        name = "bench",
        srcs = ["Bench.cpp"],
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
        name = "timedBench",
        srcs = ["TimedBench.cpp"],
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
