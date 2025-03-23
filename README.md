# adhole

Network wide ad blocker

## Setup
Prerequisites:
- [Bazel](https://bazel.build)
- Unix system (Linux, macOS, freebsd)

### Building
**Exe**:
```shell
bazel build //:adhole
```

**Lib**:
```shell
bazel build //:adhole_lib
```

### Running
**Exe**:
```shell
bazel run //:adhole config/config.conf
```