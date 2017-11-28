cc_binary(
        name = "bench",
        srcs = ["Bench.cpp"],
        copts = ["-pthread"],
        linkopts = ["-lpthread"],
        deps = [
                "@system_libs//:boost",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "timedBench",
        srcs = ["TimedBench.cpp"],
        copts = ["-pthread"],
        linkopts = ["-lpthread"],
        deps = [
                "@system_libs//:boost",
        ],
        visibility = ["//visibility:public"],
)
