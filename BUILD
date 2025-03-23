cc_library(
    name = "adhole_lib",
    hdrs = glob(["include/*.h"]),
    strip_include_prefix = "include",
)

cc_binary(
    name = "adhole",
    srcs = glob(["src/*.c"]),
    data = [
        "config/blocklist.txt",
        "config/config.conf",
    ],
    deps = ["//:adhole_lib"],
)
