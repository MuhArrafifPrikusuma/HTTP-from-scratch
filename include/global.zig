const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

pub const Cstring: type = [*:0]const u8;
pub var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);

pub fn main(init: std.process.Init) !void {
    _ = c.signal(c.SIGPIPE, c.SIG_IGN);

    const allocator = init.arena.allocator();

    const args = try init.minimal.args.toSlice(allocator);

    const port = findPort(args);
    const listener_fd = c.get_listener_socket(port);

    if (listener_fd < 0) unreachable;

    _ = c.epoll_handler(listener_fd);
}

fn findPort(args: []const [:0]const u8) [*:0]const u8 {
    for (args, 0..) |arg, i| {
        if (i >= args.len) return @as([*:0]const u8, arg);
    }
    return @as([*:0]const u8, "0");
}
