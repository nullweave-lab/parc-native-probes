# Supported Platforms

## Host validation

The initial parser library is validated with CMake and a C++17 compiler on Linux.

## Android target

The intended initial Android baseline is API 26 and later using the NDK. The current PR does not publish an AAR, JNI library, or prebuilt `.so` file.

Android NDK-supported ABI names are:

- `armeabi-v7a`;
- `arm64-v8a`;
- `x86`;
- `x86_64`.

A future binary release must list which of those ABIs were built and tested. Unsupported ABIs must be reported as capability gaps rather than silently omitted.

## Observation boundary

Current collectors observe the calling process's procfs and mount namespace. Higher-privilege software may mediate those views. This limitation is part of every observation.
