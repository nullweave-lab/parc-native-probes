#include "parc/probe.hpp"

#include <fstream>
#include <string>
#include <utility>

namespace parc {
namespace {

std::string number(std::size_t value) {
    return std::to_string(value);
}

Observation unavailable(std::string id, std::string kind, std::string reason) {
    Observation result;
    result.id = std::move(id);
    result.kind = std::move(kind);
    result.producer = "parc-native-probes";
    result.boundary = "application-process-linux-procfs";
    result.status = ProbeStatus::failed;
    result.limitations.push_back(std::move(reason));
    return result;
}

}  // namespace

Observation collect_self_maps(const ChallengeContext& context) {
    (void)context;
    std::ifstream input("/proc/self/maps");
    if (!input) {
        return unavailable("proc-self-maps", "parc.runtime.proc_maps.summary", "proc-self-maps-unavailable");
    }
    const auto summary = parse_proc_maps(input);
    Observation result;
    result.id = "proc-self-maps";
    result.kind = "parc.runtime.proc_maps.summary";
    result.producer = "parc-native-probes";
    result.boundary = "application-process-linux-procfs";
    result.status = ProbeStatus::present;
    result.claims = {
        {"records", number(summary.records)},
        {"executableRecords", number(summary.executable_records)},
        {"anonymousExecutableRecords", number(summary.anonymous_executable_records)},
        {"deletedBackingRecords", number(summary.deleted_backing_records)},
        {"malformedRecords", number(summary.malformed_records)},
    };
    result.limitations = {
        "self-view-only",
        "procfs-may-be-mediated-by-higher-privilege-software",
        "summary-is-not-a-root-or-hook-verdict",
    };
    return result;
}

Observation collect_self_mountinfo(const ChallengeContext& context) {
    (void)context;
    std::ifstream input("/proc/self/mountinfo");
    if (!input) {
        return unavailable("proc-self-mountinfo", "parc.runtime.mountinfo.summary", "proc-self-mountinfo-unavailable");
    }
    const auto summary = parse_proc_mountinfo(input);
    Observation result;
    result.id = "proc-self-mountinfo";
    result.kind = "parc.runtime.mountinfo.summary";
    result.producer = "parc-native-probes";
    result.boundary = "application-process-mount-namespace";
    result.status = ProbeStatus::present;
    result.claims = {
        {"records", number(summary.records)},
        {"overlayRecords", number(summary.overlay_records)},
        {"procRecords", number(summary.proc_records)},
        {"tmpfsRecords", number(summary.tmpfs_records)},
        {"malformedRecords", number(summary.malformed_records)},
    };
    result.limitations = {
        "current-mount-namespace-only",
        "namespace-and-procfs-may-be-controlled-by-higher-privilege-software",
        "filesystem-type-counts-are-not-a-compromise-verdict",
    };
    return result;
}

Observation collect_self_status(const ChallengeContext& context) {
    (void)context;
    std::ifstream input("/proc/self/status");
    if (!input) {
        return unavailable("proc-self-status", "parc.runtime.proc_status.summary", "proc-self-status-unavailable");
    }
    const auto summary = parse_proc_status(input);
    Observation result;
    result.id = "proc-self-status";
    result.kind = "parc.runtime.proc_status.summary";
    result.producer = "parc-native-probes";
    result.boundary = "application-process-linux-procfs";
    result.status = ProbeStatus::present;
    if (summary.tracer_pid_present) {
        result.claims["tracerPid"] = std::to_string(summary.tracer_pid);
    }
    if (summary.no_new_privs_present) {
        result.claims["noNewPrivs"] = std::to_string(summary.no_new_privs);
    }
    if (summary.seccomp_present) {
        result.claims["seccomp"] = std::to_string(summary.seccomp);
    }
    result.claims["malformedRecords"] = number(summary.malformed_records);
    result.limitations = {
        "self-view-only",
        "status-values-may-be-mediated-or-raced",
        "tracerpid-zero-does-not-prove-absence-of-instrumentation",
    };
    return result;
}

}  // namespace parc
