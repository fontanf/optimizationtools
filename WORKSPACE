load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
new_git_repository(
    name = "json",
    build_file_content = """
cc_library(
        name = "json",
        hdrs = ["single_include/nlohmann/json.hpp"],
        visibility = ["//visibility:public"],
        strip_include_prefix = "single_include/"
)
""",
    remote = "https://github.com/nlohmann/json",
    tag = "v3.7.0",
)

