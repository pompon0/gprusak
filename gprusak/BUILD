package(
    default_visibility = ["//visibility:public"],
)

cc_binary(
    name = "limit",
    srcs = ["limit.cc"],
    deps = [":utils"],
    features = ["fully_static_link"],
)

cc_library(
    name = "utils",
    hdrs = [
        "bazel.h",
        "digits.h",
        "log.h",
        "read_file.h",
        "short.h",
        "string.h",
        "utf.h",
        "types.h",
        "errors.h",
        "geom.h",
        "lex.h",
        "exec.h",
        "typeset.h",
    ],
    srcs = ["log.cc"],
)

cc_test(
  name = "utils_test",
  srcs = ["utils_test.cc"],
  deps = [
    ":utils",
    "@gtest//:gtest_main",
  ],
)

cc_library(
    name = "ctx",
    hdrs = [
      "ctx.h",
      "group.h",
    ],
    deps = [
      ":utils",
      "//gprusak/sys:raw",
      "@abseil//absl/time:time",
    ],
)

cc_test(
    name = "ctx_test",
    srcs = ["ctx_test.cc"],
    deps = [
        ":ctx",
        "@gtest//:gtest_main",
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

cc_test(
    name = "number_theory_test",
    srcs = ["number_theory_test.cc"],
    deps = [
        ":number_theory",
        ":enum_flag",
        "@gtest//:gtest_main",
    ],
)

cc_test(
  name = "utf_test",
  srcs = ["utf_test.cc"],
  deps = [
    ":utils",
    "@gtest//:gtest_main",
  ],
)

cc_test(
  name = "errors_test",
  srcs = ["errors_test.cc"],
  deps = [
    ":utils",
    "@gtest//:gtest_main",
  ],
)

cc_binary(
  name = "embed",
  srcs = ["embed.cc"],
  deps = [
    ":utils",
    "@abseil//absl/flags:flag",
    "@abseil//absl/flags:parse",
  ],
)
