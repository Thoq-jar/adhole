cc_library(
    name = "adhole_lib",
    srcs = glob(["src/*.c"]),
    hdrs = glob(["include/*.h"]),
    strip_include_prefix = "include",
    includes = ["include"],
)

cc_binary(
    name = "adhole",
    deps = [":adhole_lib"],
    data = [
        "config/blocklist.txt",
        "config/config.conf",
    ],
)
