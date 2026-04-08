load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_library",
    "envoy_package",
)

licenses(["notice"])  # Apache 2

envoy_package()

envoy_cc_library(
    name = "npds_api_lib",
    srcs = [
        "npds_api.cc",
    ],
    hdrs = [
        "npds_api.h",
    ],
    repository = "@envoy",
    deps = [
        "//cilium:grpc_subscription_lib",
        "//cilium:network_policy_lib",
        "//cilium/api:npds_cc_proto",
        "@envoy//envoy/config:subscription_interface",
        "@envoy//envoy/init:manager_interface",
        "@envoy//envoy/stats:stats_interface",
        "@envoy//source/common/common:assert_lib",
        "@envoy//source/common/common:logger_lib",
        "@envoy//source/common/config:subscription_base_interface",
        "@envoy//source/common/init:target_lib",
        "@envoy//source/common/protobuf:utility_lib",
    ],
)
