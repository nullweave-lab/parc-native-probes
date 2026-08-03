#include "parc/probe.hpp"

#include <cassert>
#include <sstream>

int main() {
    {
        std::istringstream input(
            "1000-2000 r-xp 00000000 00:00 0 /system/lib64/libc.so\n"
            "2000-3000 rw-p 00000000 00:00 0 [anon:heap]\n"
            "3000-4000 r-xp 00000000 00:00 0 [anon:jit-cache]\n"
            "4000-5000 r-xp 00000000 00:00 0 /data/local/tmp/test.so (deleted)\n"
            "malformed\n");
        const auto summary = parc::parse_proc_maps(input);
        assert(summary.records == 4);
        assert(summary.executable_records == 3);
        assert(summary.anonymous_executable_records == 1);
        assert(summary.deleted_backing_records == 1);
        assert(summary.malformed_records == 1);
    }
    {
        std::istringstream input(
            "29 23 0:25 / / rw,relatime - overlay overlay rw\n"
            "30 23 0:26 / /proc rw,nosuid - proc proc rw\n"
            "31 23 0:27 / /dev rw,nosuid - tmpfs tmpfs rw\n"
            "malformed\n");
        const auto summary = parc::parse_proc_mountinfo(input);
        assert(summary.records == 3);
        assert(summary.overlay_records == 1);
        assert(summary.proc_records == 1);
        assert(summary.tmpfs_records == 1);
        assert(summary.malformed_records == 1);
    }
    {
        std::istringstream input("Name:\ttest\nTracerPid:\t42\nNoNewPrivs:\t1\nSeccomp:\t2\n");
        const auto summary = parc::parse_proc_status(input);
        assert(summary.tracer_pid_present && summary.tracer_pid == 42);
        assert(summary.no_new_privs_present && summary.no_new_privs == 1);
        assert(summary.seccomp_present && summary.seccomp == 2);
        assert(summary.malformed_records == 0);
    }
    return 0;
}
