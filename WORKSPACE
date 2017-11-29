new_local_repository(
    name = "boost",
    path = "/usr/lib/x86_64-linux-gnu",
    build_file_content = """
cc_library(
    name = "system",
    srcs = ["libboost_system.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "filesystem",
    srcs = ["libboost_filesystem.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "program_options",
    srcs = ["libboost_program_options.so"],
    visibility = ["//visibility:public"],
)
""",
)

