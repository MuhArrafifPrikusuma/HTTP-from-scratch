const std = @import("std");
const net = @import("common.zig");
const Io = @This();

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
    func: [20]?*const fn (w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) anyerror!void = @splat(null),
};

pub var methGET: Routing = .{};
pub var methPOST: Routing = .{};
pub var methPUT: Routing = .{};
pub var methDELETE: Routing = .{};

pub fn Reader(fd: c_int, from_addr: net.Cstring, ioptr: *anyopaque) !*anyopaque {
    const io: *std.Io = @ptrCast(@alignCast(ioptr));

    var bufout: [c.MAX_READ]u8 = undefined;
    var writer = std.Io.File.stdout().writer(io.*, &bufout);
    const stdout: *std.Io.Writer = &writer.interface;

    var ctx = try net.ReadContext.init(std.heap.smp_allocator);
    const allocator = ctx.arena.allocator();

    const bytes_read: usize = @intCast(c.read(fd, &ctx.buffer[0], c.MAX_READ));
    ctx.readBuffer = try allocator.dupe(u8, ctx.buffer[0..bytes_read]);

    try stdout.print(
        "{s}read: {d} Bytes\nfrom: {s}\nContent:\n{s}",
        .{ c.COLOR_BLUE, bytes_read, from_addr, c.COLOR_RESET },
    );

    try stdout.flush();
    const request = try net.Parse.parseHTTP(ctx, io);

    return request;
}

/// generate response based on user GET request
fn handlerGET(request: *net.Parse.HttpTemplate, response: *std.ArrayList(u8), arrayAllocator: std.mem.Allocator) void {
    const httpResponse = net.Parse.HttpTemplate.init(std.heap.smp_allocator) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    };

    // NOTE: move this to the caller(writer)  later
    var lineBuf: net.Parse.Line = undefined;
    net.Parse.parseRLine(&lineBuf, request.routing.RequestLine) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    };

    var writer: net.ResponseWriter = .{ .Http = httpResponse };
    const allocator = writer.Http.arena.allocator();

    for (methGET.path, 0..) |path, i| {
        if (path == null) continue;

        if (std.ascii.eqlIgnoreCase(path.?, lineBuf.path.?)) {
            if (methGET.func[i]) |func| {
                func(&writer, request, allocator) catch |err| std.debug.print("{any}\n", .{err});
            } else {
                std.debug.print("no function found in: {s}\n", .{methGET.path[i].?});
            }
        }
    }

    // NOTE: also move this buffer to the caller
    writer.FormatHttp(response, arrayAllocator) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    };
}

pub fn Writer(fd: c_int, to_addr: net.Cstring, requestPtrFromC: *anyopaque, ioptr: *anyopaque) !void {
    const io: *std.Io = @ptrCast(@alignCast(ioptr));
    const request: *net.Parse.HttpTemplate = @ptrCast(@alignCast(requestPtrFromC));
    defer request.deinit();

    var buffer: [1024]u8 = undefined;
    var wOut = std.Io.File.stdout().writer(io.*, &buffer);
    const stdout = &wOut.interface;

    const allocator = std.heap.smp_allocator;
    var response = std.ArrayList(u8).empty;
    defer response.deinit(allocator);

    // NOTE: move handle get to another function later too
    Io.handlerGET(request, &response, allocator);

    const bytes_send: usize = @intCast(c.write(fd, &response.items[0], response.items.len));
    try stdout.print("send {d}Bytes of data to: fd{d}:{s}\n", .{ bytes_send, fd, to_addr });
    try stdout.flush();
}
