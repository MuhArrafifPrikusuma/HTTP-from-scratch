const std = @import("std");
const lib = @import("common.zig");
const net = @import("net.zig");

const c = @cImport({
    @cInclude("../include/common.h");
});

const ServerErr = error{
    RequestLineIsRequired,
    UnknownRequest,
};
// comptime variable to determine path and method for user API
const Routing = struct {
    path: [20]?[]const u8 = @splat(null),
    func: [20]?*const fn (w: *net.ResponseWriter) void = @splat(null),
};

pub var methGET: Routing = .{};
pub var methPOST: Routing = .{};
pub var methPUT: Routing = .{};
pub var methDELETE: Routing = .{};

pub fn Reader(fd: c_int, from_addr: lib.Cstring, ioptr: *anyopaque) !*anyopaque {
    const io: *std.Io = @ptrCast(@alignCast(ioptr));

    var bufout: [c.MAX_READ]u8 = undefined;
    var writer = std.Io.File.stdout().writer(io.*, &bufout);
    const stdout: *std.Io.Writer = &writer.interface;

    var ctx = try lib.ReadContext.init(std.heap.smp_allocator);
    const allocator = ctx.arena.allocator();

    const bytes_read: usize = @intCast(c.read(fd, &ctx.buffer[0], c.MAX_READ));
    ctx.readBuffer = try allocator.dupe(u8, ctx.buffer[0..bytes_read]);

    std.debug.print("time: {any}\n", .{std.Io.Clock.now(std.Io.Clock.real, io.*)});

    try stdout.print(
        "{s}read: {d} Bytes\nfrom: {s}\nContent:\n{s}",
        .{ c.COLOR_BLUE, bytes_read, from_addr, c.COLOR_RESET },
    );

    try stdout.flush();
    const request = try lib.Parse.parseHTTP(ctx, io);

    return request;
}
//
// pub fn Writer(fd: c_int, to_addr: lib.Cstring, requestPtrFromC: *anyopaque) !void {
//     const request: *lib.Parse.HttpTemplate = @ptrCast(@alignCast(requestPtrFromC));
//     defer request.deinit();
// }
// fn handleGET(req_line: *lib.Parse.Line, request: *lib.Parse.HttpTemplate) !void {}

// fn determineRequest(request: *lib.Parse.HttpTemplate) !void {
//     const req_line_unparsed = request.routing.RequestLine orelse return ServerErr.RequestLineIsRequired;
//     var requestLine_buffer: lib.Parse.Line = .{};
//     lib.Parse.parseRLine(&requestLine_buffer, req_line_unparsed);
//
//     switch (requestLine_buffer.method) {
//         lib.Parse.Methods.GET => handleGET(&requestLine_buffer, request),
//         else => return ServerErr.UnknownRequest,
//     }
// }
