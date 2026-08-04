#include "parc/probe.hpp"

#include <jni.h>

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string escape_json(const std::string& value) {
    std::ostringstream output;
    for (const unsigned char character : value) {
        switch (character) {
            case '"': output << "\\\""; break;
            case '\\': output << "\\\\"; break;
            case '\b': output << "\\b"; break;
            case '\f': output << "\\f"; break;
            case '\n': output << "\\n"; break;
            case '\r': output << "\\r"; break;
            case '\t': output << "\\t"; break;
            default:
                if (character < 0x20) {
                    output << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                           << static_cast<int>(character) << std::dec;
                } else {
                    output << character;
                }
        }
    }
    return output.str();
}

const char* status_name(parc::ProbeStatus status) {
    switch (status) {
        case parc::ProbeStatus::present: return "present";
        case parc::ProbeStatus::unsupported: return "unsupported";
        case parc::ProbeStatus::failed: return "failed";
        case parc::ProbeStatus::indeterminate: return "indeterminate";
        case parc::ProbeStatus::withheld: return "withheld";
    }
    return "indeterminate";
}

std::uint64_t fnv1a(const std::vector<std::uint8_t>& bytes) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (const auto byte : bytes) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    return hash;
}

void write_string_array(std::ostringstream& output, const std::vector<std::string>& values) {
    output << '[';
    bool first = true;
    for (const auto& value : values) {
        if (!first) output << ',';
        first = false;
        output << '"' << escape_json(value) << '"';
    }
    output << ']';
}

void write_observation(std::ostringstream& output, const parc::Observation& observation) {
    output << '{'
           << "\"id\":\"" << escape_json(observation.id) << "\","
           << "\"kind\":\"" << escape_json(observation.kind) << "\","
           << "\"producer\":\"" << escape_json(observation.producer) << "\","
           << "\"boundary\":\"" << escape_json(observation.boundary) << "\","
           << "\"status\":\"" << status_name(observation.status) << "\","
           << "\"claims\":{";
    bool first = true;
    for (const auto& [key, value] : observation.claims) {
        if (!first) output << ',';
        first = false;
        output << '"' << escape_json(key) << "\":\"" << escape_json(value) << '"';
    }
    output << "},\"limitations\":";
    write_string_array(output, observation.limitations);
    output << '}';
}

}  // namespace

extern "C" JNIEXPORT jstring JNICALL
Java_org_nullweave_parc_android_controlplane_NativeRuntimeProbe_collectJson(
    JNIEnv* environment,
    jobject,
    jbyteArray challenge_array) {
    std::vector<std::uint8_t> challenge;
    if (challenge_array != nullptr) {
        const auto size = environment->GetArrayLength(challenge_array);
        challenge.resize(static_cast<std::size_t>(size));
        if (size > 0) {
            environment->GetByteArrayRegion(
                challenge_array,
                0,
                size,
                reinterpret_cast<jbyte*>(challenge.data()));
            if (environment->ExceptionCheck()) return nullptr;
        }
    }

    parc::ChallengeContext context;
    context.challenge = challenge;
    context.sequence = 1;
    const std::vector<parc::Observation> observations = {
        parc::collect_self_maps(context),
        parc::collect_self_mountinfo(context),
        parc::collect_self_status(context),
    };

    std::ostringstream output;
    output << '{'
           << "\"implementation_version\":\"0.1.0\","
           << "\"challenge_length\":" << challenge.size() << ','
           << "\"challenge_tag_fnv1a64\":\""
           << std::hex << std::setw(16) << std::setfill('0') << fnv1a(challenge) << std::dec
           << "\",\"observations\":[";
    bool first = true;
    for (const auto& observation : observations) {
        if (!first) output << ',';
        first = false;
        write_observation(output, observation);
    }
    output << "],\"limitations\":["
           << "\"challenge-tag-is-correlation-only-not-a-cryptographic-binding\","
           << "\"higher-privilege-software-may-mediate-the-observed-process-view\""
           << "]}";

    const auto json = output.str();
    return environment->NewStringUTF(json.c_str());
}
