load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
  name = "bazel_skylib",
  urls = [
    "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
    "https://github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
  ],
  sha256 = "c6966ec828da198c5d9adbaa94c05e3a1c7f21bd012a0b29ba8ddbccb2c93b0d",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()


###############################
# LLVM

BAZEL_ZIG_CC_VERSION = "v0.4.2"

http_archive(
    name = "bazel-zig-cc",
    sha256 = "135f906ebf936a282b53e2d4516cdc86a283880ac4d6f3399f6fced2cd86c768",
    strip_prefix = "bazel-zig-cc-{}".format(BAZEL_ZIG_CC_VERSION),
    urls = ["https://git.sr.ht/~motiejus/bazel-zig-cc/archive/{}.tar.gz".format(BAZEL_ZIG_CC_VERSION)],
)

load("@bazel-zig-cc//toolchain:defs.bzl", zig_register_toolchains = "register_toolchains")

zig_register_toolchains(register = [
    "x86_64-linux-musl",
])

################################
# BAZEL

# "com_google_protobuf" declares some terribly old version of skylib,
# forcing the newest version beforehead
http_archive(
    name = "bazel_skylib",
    sha256 = "e5d90f0ec952883d56747b7604e2a15ee36e288bb556c3d0ed33e818a4d971f2",
    strip_prefix = "bazel-skylib-1.0.2",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/1.0.2.tar.gz"],
)

################################
# PROTO

http_archive(
    name = "rules_proto",
    # sha256 = "602e7161d9195e50246177e7c55b2f39950a9cf7366f74ed5f22fd45750cd208",
    sha256 = "9fc210a34f0f9e7cc31598d109b5d069ef44911a82f507d5a88716db171615a8",
    strip_prefix = "rules_proto-f7a30f6f80006b591fa7c437fe5a951eb10bcbcf",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/f7a30f6f80006b591fa7c437fe5a951eb10bcbcf.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/f7a30f6f80006b591fa7c437fe5a951eb10bcbcf.tar.gz",
    ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

################################
# C++

http_archive(
    name = "gtest",
    strip_prefix = "googletest-release-1.8.1",
    urls = ["https://github.com/google/googletest/archive/release-1.8.1.zip"],
    sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7",
)

http_archive(
    name = "abseil",
    strip_prefix = "abseil-cpp-c59e7e59f5d29619ddc07fcb59be3dcba9585814",
    urls = ["https://github.com/abseil/abseil-cpp/archive/c59e7e59f5d29619ddc07fcb59be3dcba9585814.zip"],
    sha256 = "d33deb736222c56b81c1125cb761d7ae1762b96a30a2e5dca1cf22f71c86c68f"
)
