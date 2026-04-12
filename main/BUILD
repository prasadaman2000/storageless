load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")

cc_library(
    name = "packet",
    hdrs = ["packet.h"],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [":packet"],
)

cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [":packet"],
)

cc_library(
    name = "app",
    srcs = ["app.cc"],
    hdrs = ["app.h"],
    deps = [
        ":client",
        ":packet",
        ":server",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":app",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
    ],
)

cc_binary(
    name = "server_main",
    srcs = ["server_main.cc"],
    deps = [
        ":server",
    ],
)

cc_binary(
    name = "client_main",
    srcs = ["client_main.cc"],
    deps = [
        ":client",
    ],
)
