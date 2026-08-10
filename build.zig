const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // cpu build option
    const enable_native_arch = b.option(
        bool,
        "ENABLE_NATIVE_ARCH",
        "Optimize for the build machine's CPU (-march=native)",
    ) orelse false;

    // C specific compiler flags
    const c_flags: []const []const u8 = if (enable_native_arch)
        &.{ "-std=c23", "-march=native", "-Wall", "-Wextra", "-Wpedantic", "-Werror" }
    else
        &.{ "-std=c23", "-Wall", "-Wextra", "-Wpedantic", "-Werror" };

    const server_exe = b.addExecutable(.{
        .name = "server",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    // LTO configuration on the compile step
    if (optimize == .ReleaseFast) {
        server_exe.lto = .full;
    }

    server_exe.root_module.addCSourceFiles(.{
        .files = &.{
            "include/common.c",
            "server/server.c",
        },
        .flags = c_flags,
    });

    server_exe.use_llvm = true;
    server_exe.use_lld = true;

    server_exe.root_module.addIncludePath(b.path("include"));
    server_exe.root_module.addIncludePath(b.path("server"));

    b.installArtifact(server_exe);

    // test build
    const exe_tests = b.addTest(.{
        .name = "server_tests",
        .root_module = b.createModule(.{
            .root_source_file = b.path("tests/server_test.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    exe_tests.use_llvm = true;
    exe_tests.use_lld = true;

    exe_tests.root_module.addIncludePath(b.path("include"));
    exe_tests.root_module.addIncludePath(b.path("server"));

    exe_tests.root_module.addCSourceFiles(.{
        .files = &.{
            "include/common.c",
            "server/server.c",
        },
        .flags = &.{ "-std=c23", "-DTESTING" },
    });

    const run_exe_tests = b.addRunArtifact(exe_tests);
    const test_step = b.step("test", "Run Unit tests");
    test_step.dependOn(&run_exe_tests.step);
}
