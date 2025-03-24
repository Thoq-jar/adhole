# adhole

Network wide ad blocker

This is just for me to learn!

## Setup

Prerequisites:

- [Bazel](https://bazel.build)
- Unix system (Linux, macOS, freebsd)

### System Requirements

| Arch - OS                   | Supported |
|-----------------------------|-----------|
| Arm64 - Linux               | Yes       |
| x86_64 - Linux              | Yes       |
| Arm64 macOS (Apple Silicon) | Yes       |
| Arm64 macOS (Apple Silicon) | Yes       |
| x86_64 macOS (Intel)        | Yes       |
| Arm64 Windows               | No        |
| x86_64 Windows              | No        |

| Minimum Requirements   | Recommend Requirements      |
|------------------------|-----------------------------|
| CPU: Dual (2) Core CPU | Quad (4) core CPU or better |
| Memory: 4GB DDR4       | 8GB DDR4 or better          |
| NIC: 1GB duplex¹       | 1GB duplex or better¹       |

¹This depends on your internet speed!

Recommended devices
- An old laptop you have lying around
- Raspberry Pi 5 (4GB or 8GB)
- [MINISFORUM UN100P](https://www.amazon.com/MINISFORUM-Ethernet-Business-Home-Server-Firewall/dp/B0CQYTB568?th=1)

Recommended OS's:
- Anything Debian/Ubuntu Based so
  - [Linux Mint](https://linuxmint.com/)
  - [Raspbian](https://www.raspberrypi.com/software/)
  - [Ubuntu](https://ubuntu.com/)
  - [Debian](https://www.debian.org/)

These are in order of what I recommend for beginners *and* stability.

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
sudo ./bazel-bin/adhole /etc/adhole/config.conf # Or custom path to config
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