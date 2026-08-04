# parc-native-probes

Auditable C++17 foundation for public PARC runtime measurement probes.

The initial implementation contains parsers and collectors for bounded summaries of:

- `/proc/self/maps`;
- `/proc/self/mountinfo`;
- `/proc/self/status`.

The collectors report provenance, control boundary, claims, and limitations. They do not classify a device as rooted, hooked, clean, trusted, or untrusted.

## Build and test

```sh
cmake -S . -B build -DPARC_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The parser tests compile and pass on a standard Linux C++17 toolchain. Android integration will use the NDK and an explicit JNI adapter in a later PR.

## Public boundary

This repository contains basic, reviewable observations and fixture-based parsers. It must not contain private production signatures, stealth rules, unresolved bypass chains, credentials, or patent-confidential mechanisms.

## License

Apache License 2.0. See `LICENSE`.
