cc_library(
    name = "poly_map",
    hdrs = ["include/msd/poly_map.hpp"],
    includes = ["include"],
    visibility = ["//visibility:public"],
)

cc_test(
  name = "poly_map_test",
  size = "small",
  srcs = ["tests/poly_map_test.cpp"],
  copts = ["--std=c++17 -Wall -Wextra -Wpedantic -Werror"],
  deps = [
        ":poly_map",
        "@gtest//:gtest_main",
    ],
)
