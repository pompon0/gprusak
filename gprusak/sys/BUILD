package(
    default_visibility = ["//visibility:public"],
)


cc_library(
  name = "raw",
  hdrs = ["raw.h"],
  deps = ["//gprusak:utils"],
)

cc_library(
  name = "sys",
  hdrs = ["subprocess.h"],
  deps = [
    ":raw",
    "//gprusak:ctx",
  ],
)
