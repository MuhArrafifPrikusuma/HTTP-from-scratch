const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
}

fn serveIndex(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriterStatus("work", allocator);

    try w.WriteBody("hello", allocator);
    _ = r;
}
fn serveIndex1(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteBody("yes yes no\n", allocator);
    _ = r;
}
