#include "parc/probe.hpp"

#include <charconv>
#include <sstream>
#include <string_view>

namespace parc {
namespace {

bool parse_u64(std::string_view text, std::uint64_t& output) {
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
        text.remove_prefix(1);
    }
    while (!text.empty() &&
           (text.back() == ' ' || text.back() == '\t' || text.back() == '\r')) {
        text.remove_suffix(1);
    }
    if (text.empty()) {
        return false;
    }
    const auto* begin = text.data();
    const auto* end = text.data() + text.size();
    const auto result = std::from_chars(begin, end, output);
    return result.ec == std::errc{} && result.ptr == end;
}

}  // namespace

MapsSummary parse_proc_maps(std::istream& input) {
    MapsSummary summary;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream fields(line);
        std::string range;
        std::string permissions;
        std::string offset;
        std::string device;
        std::string inode;
        if (!(fields >> range >> permissions >> offset >> device >> inode)) {
            ++summary.malformed_records;
            continue;
        }
        ++summary.records;
        const bool executable = permissions.size() >= 3 && permissions[2] == 'x';
        if (executable) {
            ++summary.executable_records;
        }

        std::string path;
        std::getline(fields, path);
        const auto first = path.find_first_not_of(" \t");
        if (first != std::string::npos) {
            path.erase(0, first);
        } else {
            path.clear();
        }

        const bool anonymous = path.empty() || path.front() == '[';
        if (executable && anonymous) {
            ++summary.anonymous_executable_records;
        }
        if (path.find(" (deleted)") != std::string::npos) {
            ++summary.deleted_backing_records;
        }
    }
    return summary;
}

MountSummary parse_proc_mountinfo(std::istream& input) {
    MountSummary summary;
    std::string line;
    while (std::getline(input, line)) {
        const auto separator = line.find(" - ");
        if (separator == std::string::npos) {
            ++summary.malformed_records;
            continue;
        }
        const std::string post = line.substr(separator + 3);
        std::istringstream fields(post);
        std::string filesystem;
        if (!(fields >> filesystem)) {
            ++summary.malformed_records;
            continue;
        }
        ++summary.records;
        if (filesystem == "overlay") {
            ++summary.overlay_records;
        } else if (filesystem == "proc") {
            ++summary.proc_records;
        } else if (filesystem == "tmpfs") {
            ++summary.tmpfs_records;
        }
    }
    return summary;
}

StatusSummary parse_proc_status(std::istream& input) {
    StatusSummary summary;
    std::string line;
    while (std::getline(input, line)) {
        const auto separator = line.find(':');
        if (separator == std::string::npos) {
            continue;
        }
        const std::string_view key(line.data(), separator);
        const std::string_view value(line.data() + separator + 1, line.size() - separator - 1);
        std::uint64_t parsed = 0;
        if (key == "TracerPid") {
            summary.tracer_pid_present = true;
            if (!parse_u64(value, parsed)) {
                ++summary.malformed_records;
            } else {
                summary.tracer_pid = parsed;
            }
        } else if (key == "NoNewPrivs") {
            summary.no_new_privs_present = true;
            if (!parse_u64(value, parsed)) {
                ++summary.malformed_records;
            } else {
                summary.no_new_privs = parsed;
            }
        } else if (key == "Seccomp") {
            summary.seccomp_present = true;
            if (!parse_u64(value, parsed)) {
                ++summary.malformed_records;
            } else {
                summary.seccomp = parsed;
            }
        }
    }
    return summary;
}

}  // namespace parc
