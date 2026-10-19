load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "bazel-project",
    srcs = ["src/main.cpp"],
    deps = [
        "@poly_map//:poly_map",
    ],
    copts = ["--std=c++17"],
)
