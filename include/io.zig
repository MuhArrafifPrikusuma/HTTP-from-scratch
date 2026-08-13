const std = @import("std");
const g = @import("global.zig");
const c = @cImport({
    @cInclude("../include/common.h");
});

pub export fn Reader(fd: c_int, from_addr: g.Cstring) *anyopaque {
    const allocator = g.arena.allocator();
    const read_buf = allocator.alloc(u8, c.MAX_READ) catch unreachable;

    const bytes_read: usize = @intCast(c.read(fd, read_buf.ptr, c.MAX_READ));

    Print2(
        "read: {d} Bytes\nfrom: {s}\nContent: {s}\n",
        .{ bytes_read, from_addr, read_buf },
    ) catch unreachable;

    return read_buf.ptr;
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
