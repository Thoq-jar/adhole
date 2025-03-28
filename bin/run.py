#!/usr/bin/env python3

import os

def main():
    os.system("bazel build //:adhole")
    os.system("clear")
    os.system("./bazel-bin/adhole config/config.conf")

if __name__ == "__main__":
    main()
