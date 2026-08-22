const std = @import("std");
const net = @import("library/common.zig");

pub fn start() void {
    net.handleFunc(net.Method.GET, "/", serveIndex);
    net.handleFunc(net.Method.GET, "/no", serveIndex1);
    net.handleFunc(net.Method.GET, "/idk", idkFunc);
    net.handleFunc(net.Method.GET, "/node_modules/htmx.org/dist/htmx.min.js", sendJavascript);
    net.handleFunc(net.Method.GET, "/list.html", serveList);
    net.handleFunc(net.Method.GET, "/index.html", routeBack);
    net.handleFunc(net.Method.POST, "/submit", takeFromPOST);
    net.handleFunc(net.Method.GET, "/testpost", sendDataToList);
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
    // FIXME: maybe replace later on with aaaaaaaaaa....... io from the handleFunc function
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

var tmp: std.ArrayList([]const u8) = .empty;
fn takeFromPOST(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    const tmpAllocator = std.heap.smp_allocator;
    var split = std.mem.splitSequence(u8, r.body.?, "t=");
    _ = split.next() orelse return error.NoBody;
    const actual = split.next() orelse "";

    const body = try tmpAllocator.dupe(u8, actual);
    var list = &tmp;

    try list.append(tmpAllocator, body);
    for (tmp.items) |value| {
        std.debug.print("this is: {s}\n", .{value});
    }

    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriteBody("data sent to server", allocator);
}

fn routeBack(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;
    var thread = std.Io.Threaded.init_single_threaded;
    const io = thread.io();

    const file = try std.Io.Dir.cwd().openFile(io, "demo/index.html", .{});
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

    const index = try std.fmt.bufPrint(&buf, "{s}", .{content});

    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriteBody(index, allocator);
}

// FIX: this shit won't work if the first request is not exactly 4bytes
fn sendDataToList(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;

    std.debug.print("before again: {s}\n", .{tmp.items[0]});
    var body = std.ArrayList([]const u8).empty;
    const bodyAllocator = std.heap.smp_allocator;
    var bodyPtr = &body;
    var buf: [4096]u8 = undefined;

    defer {
        for (body.items) |value| {
            bodyAllocator.free(value);
        }
        body.deinit(bodyAllocator);
    }

    std.debug.print("beforehand {s}\n", .{tmp.items[0]});
    // generate list
    for (tmp.items) |li| {
        std.debug.print("this function does ran; {s}\n", .{li});
        const formattedList = try std.fmt.bufPrint(&buf, "<li>{s}</li>", .{li});
        std.debug.print("{s}\n", .{formattedList});

        const owned_str = try bodyAllocator.dupe(u8, formattedList);
        try bodyPtr.append(bodyAllocator, owned_str);
        std.debug.print("now it turns to {s}\n", .{body.items[0]});
    }

    const single_string = try std.mem.join(bodyAllocator, "\n", body.items);
    std.debug.print("what is this: \n{s}\n", .{single_string});
    defer bodyAllocator.free(single_string);

    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriteBody(single_string, allocator);
}

fn serveList(w: *net.ResponseWriter, r: *net.Parse.HttpTemplate, allocator: std.mem.Allocator) !void {
    _ = r;
    var thread = std.Io.Threaded.init_single_threaded;
    const io = thread.io();

    const file = try std.Io.Dir.cwd().openFile(io, "demo/list.html", .{});
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

    const list = try std.fmt.bufPrint(&buf, "{s}", .{content});

    try w.WriterStatus(net.ResponseStatus.OK, allocator);
    try w.WriteContentType("text/html", allocator);
    try w.WriteBody(list, allocator);
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
