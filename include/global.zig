const std = @import("std");
const lib = @import("zlib");
const c = @cImport({
    @cInclude("server.h");
});

pub fn main(init: std.process.Init) !void {
    var io = init.io;
    _ = c.signal(c.SIGPIPE, c.SIG_IGN);

    const allocator = init.arena.allocator();

    const args = try init.minimal.args.toSlice(allocator);

    const port = findPort(args);
    const listener_fd = c.get_listener_socket(port);

    if (listener_fd < 0) unreachable;

    _ = c.epoll_handler(listener_fd, &io);
}

fn findPort(args: []const [:0]const u8) lib.Cstring {
    if (args.len < 2) return @as(lib.Cstring, "0");
    for (args, 0..) |arg, i| {
        if (i >= args.len - 1) return @as(lib.Cstring, arg);
    }
    return @as(lib.Cstring, "0");
}
