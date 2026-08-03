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
        &.{ "-std=c23", "-march=native" }
    else
        &.{"-std=c23"};

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
            "server/server_error.c",
        },
        .flags = c_flags,
    });

    server_exe.root_module.addIncludePath(b.path("include"));
    server_exe.root_module.addIncludePath(b.path("server"));

    b.installArtifact(server_exe);
}
