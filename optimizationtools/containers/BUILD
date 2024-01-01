cc_library(
        name = "containers",
        hdrs = [
                "indexed_set.hpp",
                "indexed_map.hpp",
                "doubly_indexed_map.hpp",
                "indexed_binary_heap.hpp",
                "sorted_on_demand_array.hpp",
                "partial_set.hpp",
                "space_efficient_array.hpp",
        ],
        srcs = [ ],
        visibility = ["//visibility:public"],
)

cc_test(
        name = "test",
        srcs = ["space_efficient_array_test.cpp"],
        deps = [
                ":containers",
                "@googletest//:gtest_main",
        ],
        timeout = "short",
)
