const std = @import("std");
const net = @import("library/net.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
}

fn serveIndex(w: *net.ResponseWriter) void {
    _ = w;
}
fn serveIndex1(w: *net.ResponseWriter) void {
    _ = w;
}
