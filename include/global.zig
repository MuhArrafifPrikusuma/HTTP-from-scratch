const std = @import("std");
const net = @import("library/common.zig");
const api = @import("api.zig");
const c = @cImport({
    @cInclude("server.h");
});

pub fn main(init: std.process.Init) !void {
    var io = init.io;
    _ = c.signal(c.SIGPIPE, c.SIG_IGN);

    const allocator = init.arena.allocator();
    api.start();

    const args = try init.minimal.args.toSlice(allocator);

    const port = findPort(args);
    const listener_fd = c.get_listener_socket(port);

    if (listener_fd < 0) unreachable;

    _ = c.epoll_handler(listener_fd, &io);
}

fn findPort(args: []const [:0]const u8) net.Cstring {
    if (args.len < 2) return @as(net.Cstring, "0");
    for (args, 0..) |arg, i| {
        if (i >= args.len - 1) return @as(net.Cstring, arg);
    }
    return @as(net.Cstring, "0");
}
