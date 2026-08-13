const std = @import("std");
const c = @cImport({
    @cInclude("../include/common.h");
});
const g = @import("global.zig");

pub export fn Reader(fd: c_int) *anyopaque {
    const allocator = g.arena.allocator();

    const read_buf = allocator.alloc(u8, c.MAX_READ) catch unreachable;

    const bytes_read: usize = @intCast(c.read(fd, read_buf.ptr, c.MAX_READ));
    std.debug.print("this is what i read: {s}\ntotal {d} bytes\n", .{ read_buf, bytes_read });

    return read_buf.ptr;
}

// pub fn Writer(fd: c_int) void {
//     const allocator = g.arena.allocator();
//     const write_buf = allocator.
// }
