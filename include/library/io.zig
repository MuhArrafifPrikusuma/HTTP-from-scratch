const std = @import("std");
const lib = @import("common.zig");

const c = @cImport({
    @cInclude("../include/common.h");
});

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
//
// fn glanceRequest(request: *lib.Parse.HttpTemplate) []bool {
//     const fields = []const ?[]const u8{
//         // routing 0-3
//         request.routing.RequestLine,
//         request.routing.Host,
//         request.routing.Connection,
//         request.routing.Upgrade,
//         // client 4-9
//         request.client.Accept,
//         request.client.AcceptCharset,
//         request.client.AcceptEncoding,
//         request.client.AcceptLanguage,
//         request.client.UserAgent,
//         request.client.DNT,
//         // content 10-13
//         request.payload.ContentType,
//         request.payload.ContentEncoding,
//         request.payload.ContentLanguage,
//         request.payload.ContentLength,
//         // body 14
//         request.body,
//     };
//     var whichExist: [15]bool = @splat(false);
//
//     for (fields, &whichExist) |v, w| {
//         if (v == null or v.?.len == 0) continue;
//         w = true;
//         std.debug.print("test: {any}\n", .{w});
//     }
//
//     return whichExist;
// }
