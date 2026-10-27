#!/bin/sh
set -eu
cd "$(dirname "$0")"
c++ -std=c++17 -O2 -fPIC -shared -fvisibility=hidden -mcpu=cortex-a53 obxd_lv2.cpp -o obxd.lv2/obxd.so
strip --strip-unneeded obxd.lv2/obxd.so
