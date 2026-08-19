const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
}

fn serveIndex(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate) void {
    _ = w;
    _ = r;
}
fn serveIndex1(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate) void {
    _ = w;
    _ = r;
}
