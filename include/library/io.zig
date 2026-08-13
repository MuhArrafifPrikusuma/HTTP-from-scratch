const std = @import("std");
const common = @import("common.zig");
const c = @cImport({
    @cInclude("../include/common.h");
});

pub fn Reader(fd: c_int, from_addr: [*:0]const u8) *anyopaque {
    var ctx = common.ReadContext.startContext(std.heap.smp_allocator) catch unreachable;

    const bytes_read: usize = @intCast(c.read(fd, &ctx.read_buf[0], c.MAX_READ));

    Print2(
        "read: {d} Bytes\nfrom: {s}\nContent: {s}\n",
        .{ bytes_read, from_addr, ctx.read_buf },
    ) catch unreachable;

    return ctx;
}

// pub export fn Writer(fd: c_int) void {
//     const allocator = g.arena.allocator();
//     const write_buf = allocator.
// }

// lovely buffered print statement to print to stdout, this should
// mainly be used for printing anything no more than 4 KB
pub fn Print2(comptime fmt: []const u8, args: anytype) !void {
    // spawn io thread
    var threaded = std.Io.Threaded.init_single_threaded;
    const io = threaded.io();

    var buf: [c.MAX_READ]u8 = undefined;
    var writer = std.Io.File.stdout().writer(io, &buf);
    const stdout = &writer.interface;

    try stdout.print(fmt, args);
    try stdout.flush();
}
