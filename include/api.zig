const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
    net.handleFunc(net.Method.GET, "/idk", idkFunc);
}

fn serveIndex(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriterStatus("200 OK", allocator);
    try w.WriteContentType("text/html", allocator);

    var buf: [1024]u8 = undefined;
    const idkWhatever = try std.fmt.bufPrint(&buf, "<html><body><h1>Zero dependency btw</h1></body></html>", .{});
    try w.WriteBody(idkWhatever, allocator);
    _ = r;
}
fn serveIndex1(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteBody("<html><body><h1>Another page</h1></body></html>", allocator);
    _ = r;
}

fn idkFunc(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;
    try w.WriteBody("hello", allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.NotFound, allocator);
}
