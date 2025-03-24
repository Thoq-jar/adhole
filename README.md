# adhole

Network wide ad blocker

## Setup
Prerequisites:
- [Bazel](https://bazel.build)
- Unix system (Linux, macOS, freebsd)

### Deploying
1. Edit the config in `config/config.conf` to run on the
desired port (likely 53) and also point to your blocklist,
if you want to use the provided blocklist, copy it over to the recommended
dir `/etc/adhole/blocklist.txt`.
2. Copy the config over to `/etc/adhole/config.conf`
and make sure its pointing to the blocklist.
3. Run: `git clone https://github.com/thoq-jar/adhole.git && cd adhole`
4. Run the following command:
```shell
set -o pipefail
command -v bazel && bazel --version
bazel build //:adhole
sudo ./bazel-out/adhole /etc/adhole/config.conf # Or custom path to config
```
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