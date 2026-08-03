#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <string>
#include <vector>

namespace parc {

enum class ProbeStatus {
    present,
    unsupported,
    failed,
    indeterminate,
    withheld,
};

struct ChallengeContext {
    std::vector<std::uint8_t> challenge;
    std::uint64_t sequence = 0;
};

struct Observation {
    std::string id;
    std::string kind;
    std::string producer;
    std::string boundary;
    ProbeStatus status = ProbeStatus::indeterminate;
    std::map<std::string, std::string> claims;
    std::vector<std::string> limitations;
};

struct MapsSummary {
    std::size_t records = 0;
    std::size_t executable_records = 0;
    std::size_t anonymous_executable_records = 0;
    std::size_t deleted_backing_records = 0;
    std::size_t malformed_records = 0;
};

struct MountSummary {
    std::size_t records = 0;
    std::size_t overlay_records = 0;
    std::size_t proc_records = 0;
    std::size_t tmpfs_records = 0;
    std::size_t malformed_records = 0;
};

struct StatusSummary {
    bool tracer_pid_present = false;
    std::uint64_t tracer_pid = 0;
    bool no_new_privs_present = false;
    std::uint64_t no_new_privs = 0;
    bool seccomp_present = false;
    std::uint64_t seccomp = 0;
    std::size_t malformed_records = 0;
};

MapsSummary parse_proc_maps(std::istream& input);
MountSummary parse_proc_mountinfo(std::istream& input);
StatusSummary parse_proc_status(std::istream& input);

Observation collect_self_maps(const ChallengeContext& context);
Observation collect_self_mountinfo(const ChallengeContext& context);
Observation collect_self_status(const ChallengeContext& context);

}  // namespace parc
