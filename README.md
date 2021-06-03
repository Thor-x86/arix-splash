# Splash Animation for ARIX OS

> NOTE: For unfinished project, only works on Linux ARM64 and x86 devices!

This repo meant for showing boot animation for currently unreleased ARIX OS. However, you can use this for any other project but please credit the author. Thanks!

## How to build

1. Install `make`, `cmake`, and `linaro`
2. Enter command `./configure` if this first time
3. Enter command `make` for your computer
4. Enter command `LINARO_PATH=<path_to_linaro> make` for ARM64 machine

## How to test

1. Use non-GUI Linux Machine like Ubuntu Server
2. Run `bin/arix-splash example`
3. To stop, hit CTRL+C
