package(
    default_visibility = ["//visibility:public"],
)

cc_binary(
    name = "limit",
    srcs = ["limit.cc"],
    deps = [":utils"],
    features = ["fully_static_link"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "utils",
    hdrs = [
        "bazel.h",
        "log.h",
        "read_file.h",
        "short.h",
        "string.h",
        "types.h",
    ],
    srcs = ["log.cc"],
)

cc_library(
    name = "ctx",
    hdrs = ["ctx.h"],
    deps = [
      ":utils",
      "@abseil//absl/time:time",
      "@abseil//absl/types:optional",
    ],
)

cc_library(
    name = "enum_flag",
    hdrs = ["enum_flag.h"],
    deps = [
      ":utils",
      "@com_google_protobuf//:protobuf",
      "@abseil//absl/strings:strings",
    ],
)

cc_library(
    name = "number_theory",
    hdrs = ["number_theory.h"],
)

cc_library(
  name = "sys",
  hdrs = ["sys.h"],
  deps = [
    ":utils",
    ":ctx",
  ],
)

cc_test(
    name = "number_theory_test",
    srcs = ["number_theory_test.cc"],
    deps = [
        ":number_theory",
        ":enum_flag",
        "@gtest//:gtest_main",
    ],
)