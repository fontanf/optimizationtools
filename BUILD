cc_binary(
        name = "bench",
        srcs = ["Bench.cpp"],
        copts = ["-pthread"],
        linkopts = ["-lpthread"],
        deps = [
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "timedBench",
        srcs = ["TimedBench.cpp"],
        copts = ["-pthread"],
        linkopts = ["-lpthread"],
        deps = [
                "@boost//:filesystem",
                "@boost//:system",
                "@boost//:program_options",
        ],
        visibility = ["//visibility:public"],
)
