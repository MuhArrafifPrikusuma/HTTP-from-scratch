const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
}

fn serveIndex(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriterStatus("200 OK", allocator);
    try w.WriteContentType("text/html", allocator);

    var buf: [1024]u8 = undefined;
    const ipAddr = try std.fmt.bufPrint(&buf, "<html><body><h1>{s}</h1></body></html>", .{
        r.routing.Host.?,
    });
    try w.WriteBody(ipAddr, allocator);
}
fn serveIndex1(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteBody("<html><body><h1>Another page</h1></body></html>", allocator);
    _ = r;
}
