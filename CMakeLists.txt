cmake_minimum_required(VERSION 3.22)
project(parc_native_probes VERSION 0.1.0 LANGUAGES CXX)

option(PARC_BUILD_TESTS "Build parser and probe tests" ON)

add_library(parc_probes STATIC
    src/proc_parsers.cpp
    src/linux_proc_probe.cpp
)
set_target_properties(parc_probes PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(parc_probes PUBLIC include)
target_compile_features(parc_probes PUBLIC cxx_std_17)
target_compile_options(parc_probes PRIVATE -Wall -Wextra -Wpedantic)

if(ANDROID)
    add_library(parc_probe_jni SHARED src/android_jni.cpp)
    target_link_libraries(parc_probe_jni PRIVATE parc_probes)
    target_include_directories(parc_probe_jni PRIVATE include)
    target_compile_features(parc_probe_jni PRIVATE cxx_std_17)
    target_compile_options(parc_probe_jni PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(PARC_BUILD_TESTS AND NOT ANDROID)
    enable_testing()
    add_executable(parc_probe_tests tests/parser_test.cpp)
    target_link_libraries(parc_probe_tests PRIVATE parc_probes)
    target_compile_features(parc_probe_tests PRIVATE cxx_std_17)
    add_test(NAME parc_probe_tests COMMAND parc_probe_tests)
endif()
