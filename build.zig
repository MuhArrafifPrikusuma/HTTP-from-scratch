const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // cpu build optional
    const enable_native_arch = b.option(
        bool,
        "ENABLE_NATIVE_ARCH",
        "Optimize for the build machine's CPU (-march=native)",
    ) orelse false;

    // compiler flags
    const c_flags: []const []const u8 = if (enable_native_arch)
        &.{ "-std=c23", "-march=native", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-fno-unwind-tables" }
    else
        &.{ "-std=c23", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-fno-unwind-tables" };

    const zlib = b.addLibrary(.{
        .linkage = .static,
        .name = "zlib",
        .root_module = b.createModule(.{
            .root_source_file = b.path("include/library/common.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    zlib.root_module.addIncludePath(b.path("include/"));

    const server_exe = b.addExecutable(.{
        .name = "server",
        .root_module = b.createModule(.{
            .root_source_file = b.path("include/global.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    // LTO configuration on the compile step

    if (optimize != .Debug) {
        server_exe.root_module.strip = true;
        server_exe.root_module.unwind_tables = .none;

        if (optimize == .ReleaseFast or optimize == .ReleaseSmall) {
            server_exe.link_gc_sections = true;

            server_exe.root_module.stack_check = false;
            server_exe.root_module.valgrind = false;
            server_exe.discard_local_symbols = true;

            server_exe.lto = .full;
            server_exe.dead_strip_dylibs = true;
        }
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

    server_exe.root_module.addIncludePath(b.path("include/"));
    server_exe.root_module.addIncludePath(b.path("server/"));
    server_exe.root_module.addIncludePath(b.path("includel/library"));
    server_exe.root_module.addImport("zlib", zlib.root_module);

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
    exe_tests.root_module.addIncludePath(b.path("include/library/"));

    exe_tests.root_module.addCSourceFiles(.{
        .files = &.{
            "include/common.c",
            "server/server.c",
        },
        .flags = &.{ "-std=c23", "-DTESTING" },
    });
    exe_tests.root_module.linkLibrary(zlib);

    const run_exe_tests = b.addRunArtifact(exe_tests);
    const test_step = b.step("test", "Run Unit tests");
    test_step.dependOn(&run_exe_tests.step);

    // ZLS

    const zls_check = b.addExecutable(.{
        .name = "server",
        .root_module = b.createModule(.{
            .root_source_file = b.path("include/global.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    zls_check.root_module.addCSourceFiles(.{
        .files = &.{
            "include/common.c",
            "server/server.c",
        },
        .flags = c_flags,
    });
    zls_check.root_module.linkLibrary(zlib);

    zls_check.root_module.addIncludePath(b.path("server"));
    zls_check.root_module.addIncludePath(b.path("include"));
    zls_check.root_module.addIncludePath(b.path("include/library/"));

    const check_step = b.step("check", "Make zls check this artifact");
    check_step.dependOn(&zls_check.step);
}
