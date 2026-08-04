# Android JNI Packaging

The native probe library now exposes an Android JNI adapter compatible with:

```text
org.nullweave.parc.android.controlplane.NativeRuntimeProbe.collectJson(byte[])
```

## Output

The JNI adapter returns bounded JSON containing:

- a non-cryptographic challenge correlation tag;
- the three structured observations produced by the existing `/proc/self/maps`, `/proc/self/mountinfo`, and `/proc/self/status` collectors;
- producer and boundary metadata;
- claims and explicit limitations;
- no raw mapped paths, mount paths, credentials, or unrelated process data.

## Android build

CI installs NDK `27.2.12479018` and cross-compiles:

- `arm64-v8a/libparc_probe_jni.so`
- `armeabi-v7a/libparc_probe_jni.so`
- `x86_64/libparc_probe_jni.so`

The libraries are uploaded as the `parc-native-probes-android-jni` artifact.

Manual example:

```sh
NDK="$ANDROID_HOME/ndk/27.2.12479018"
cmake -S . -B build-arm64 \
  -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DPARC_BUILD_TESTS=OFF
cmake --build build-arm64 --parallel
```

## Trust boundary

The probe observes only the calling process's procfs and mount namespace. Higher-privilege software may mediate or forge that view. The output is evidence for cross-checking, not a standalone root/hook verdict.

The challenge tag is FNV-1a and exists only to show that challenge bytes reached the native call. Cryptographic challenge binding must be provided by the outer signed or MAC-authenticated proof.
