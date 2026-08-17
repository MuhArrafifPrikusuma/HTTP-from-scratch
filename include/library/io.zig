const std = @import("std");
const lib = @import("common.zig");

const c = @cImport({
    @cInclude("../include/common.h");
});

pub fn Reader(fd: c_int, from_addr: [*:0]const u8, ioptr: *anyopaque) !*anyopaque {
    const io: *std.Io = @ptrCast(@alignCast(ioptr));

    var bufout: [c.MAX_READ]u8 = undefined;
    var writer = std.Io.File.stdout().writer(io.*, &bufout);
    const stdout: *std.Io.Writer = &writer.interface;

    var ctx = try lib.ReadContext.init(std.heap.smp_allocator);
    const allocator = ctx.arena.allocator();

    const bytes_read: usize = @intCast(c.read(fd, &ctx.buffer[0], c.MAX_READ));
    ctx.readBuffer = try allocator.dupe(u8, ctx.buffer[0..bytes_read]);
    try lib.Parse.parseHTTP(ctx, io);

    try stdout.print(
        "{s}read: {d} Bytes\nfrom: {s}\nContent:\n{s}{s}\n",
        .{ c.COLOR_BLUE, bytes_read, from_addr, c.COLOR_RESET, ctx.readBuffer },
    );

    try stdout.flush();

    return ctx;
}

// pub export fn Writer(fd: c_int) void {
//     const allocator = g.arena.allocator();
//     const write_buf = allocator.
// }
