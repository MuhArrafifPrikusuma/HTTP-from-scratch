const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
    net.handleFunc(net.Method.GET, "/idk", idkFunc);
    net.handleFunc(net.Method.GET, "/node_modules/htmx.org/dist/htmx.min.js", sendJavascript);
}

fn serveIndex(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    var thread = std.Io.Threaded.init_single_threaded;
    const io = thread.io();

    const file = try std.Io.Dir.cwd().openFile(io, "demo/index.html", .{});
    defer file.close(io);

    var bufRead: [4096]u8 = undefined;
    var f_reader = file.reader(io, &bufRead);
    const reader = &f_reader.interface;

    const stat = try file.stat(io);
    const content = bufRead[0..stat.size];
    try reader.readSliceAll(content);

    try w.WriterStatus("200 OK", allocator);
    try w.WriteContentType("text/html", allocator);

    var buf: [1024]u8 = undefined;
    const idkWhatever = try std.fmt.bufPrint(&buf, "{s}", .{content});
    try w.WriteBody(idkWhatever, allocator);
    _ = r;
}
fn serveIndex1(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteBody("<html><body><h1>Another page</h1></body></html>", allocator);
    _ = r;
}

fn sendJavascript(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;
    var thread = std.Io.Threaded.init_single_threaded;
    const io = thread.io();

    const file = try std.Io.Dir.cwd().openFile(io, "demo/node_modules/htmx.org/dist/htmx.min.js", .{});
    defer file.close(io);

    var bufRead: [1677716]u8 = undefined;
    var f_reader = file.reader(io, &bufRead);
    const reader = &f_reader.interface;

    const stat = try file.stat(io);
    const content = bufRead[0..stat.size];
    try reader.readSliceAll(content);

    try w.WriterStatus("200 OK", allocator);
    try w.WriteContentType("text/html", allocator);

    var buf: [1677716]u8 = undefined;

    const javascript = try std.fmt.bufPrint(&buf, "{s}", .{content});

    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteContentType("text/javascript; charset=utf-8", allocator);
    try w.WriteBody(javascript, allocator);
}

fn idkFunc(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;
    try w.WriteBody("hello", allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriterStatus(net.ResponseStatus.NotFound, allocator);
}

// fn testPOST(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {}
//
// fn name() !void {}
