const std = @import("std");

pub const Io = @import("io.zig");
pub const Parse = @import("parse.zig");

pub const Method: type = Parse.Methods;
pub const Cstring: type = [*:0]const u8;

pub const ReadContext = struct {
    arena: std.heap.ArenaAllocator,
    buffer: [4096]u8,
    readBuffer: []u8,

    pub fn init(backing_allocator: std.mem.Allocator) !*ReadContext {
        var arena = std.heap.ArenaAllocator.init(backing_allocator);
        errdefer arena.deinit();

        const allocator = arena.allocator();
        const self = try allocator.create(ReadContext);

        self.* = .{
            .arena = arena,
            .buffer = undefined,
            .readBuffer = undefined,
        };

        return self;
    }

    pub fn deinit(self: *ReadContext) void {
        const arena = self.arena;
        arena.deinit();
    }
};

export fn eReader(fd: c_int, from_addr: Cstring, ioptr: *anyopaque) ?*anyopaque {
    const ptr = Io.Reader(fd, from_addr, ioptr) catch return null;
    return ptr;
}

export fn eWriter(fd: c_int, to_addr: Cstring, requestPtrFromC: *anyopaque, ioptr: *anyopaque) void {
    Io.Writer(fd, to_addr, requestPtrFromC, ioptr) catch |err| {
        std.debug.print("failed to write data {any}\n", .{err});
    };
}

pub const ResponseStatus: struct {
    // success
    OK: []const u8 = "200 OK",
    Created: []const u8 = "201 Created",
    // client error
    BadRequest: []const u8 = "400 Bad Request",
    NotFound: []const u8 = "404 Not Found",
    // server error
    InternalServerError: []const u8 = "500 Internal Server Error",
} = .{};

pub const ResponseWriter = struct {
    // make this heap allocaated inside HttpTemplate later
    // initiate this on the main run function later
    Http: *Parse.HttpTemplate = undefined,

    pub fn WriterStatus(self: *ResponseWriter, status: []const u8, allocator: std.mem.Allocator) !void {
        var buf: [256]u8 = undefined;
        const result = try std.fmt.bufPrint(&buf, "HTTP/1.1 {s}\r\n", .{status});
        self.Http.r_line.status = try allocator.dupe(u8, result);
        self.Http.routing.Connection = try allocator.dupe(u8, "Connection: keep-alive\r\n");
        self.Http.payload.ContentType = try allocator.dupe(u8, "Content-Type: none\r\n");
        self.Http.payload.ContentLength = try allocator.dupe(u8, "Content-Length: 0\r\n");
    }

    pub fn WriteContentType(self: *ResponseWriter, ContentType: []const u8, allocator: std.mem.Allocator) !void {
        var buf: [256]u8 = undefined;
        const result = try std.fmt.bufPrint(&buf, "Content-Type: {s}\r\n", .{ContentType});
        self.Http.payload.ContentType = try allocator.dupe(u8, result);
    }

    pub fn WriteBody(self: *ResponseWriter, body: []const u8, allocator: std.mem.Allocator) !void {
        self.Http.body = body;
        var buf: [256]u8 = undefined;
        const result = try std.fmt.bufPrint(&buf, "Content-Length: {d}\r\n", .{body.len});
        self.Http.payload.ContentLength = try allocator.dupe(u8, result);
    }

    /// format all that has been written and then return the slice of http response
    pub fn FormatHttp(self: *ResponseWriter, response: *std.ArrayList(u8), allocator: std.mem.Allocator) !void {
        defer self.Http.deinit();

        // make header
        if (self.Http.r_line.status) |status| try response.appendSlice(allocator, status);
        if (self.Http.date) |date| try response.appendSlice(allocator, date);
        if (self.Http.payload.ContentType) |ct| try response.appendSlice(allocator, ct);
        if (self.Http.payload.ContentLength) |cl| try response.appendSlice(allocator, cl);
        if (self.Http.routing.Connection) |con| try response.appendSlice(allocator, con);

        try response.appendSlice(allocator, "\r\n");
        if (self.Http.body) |body| try response.appendSlice(allocator, body);
    }
};

// NOTE: the one above gave a pointer to this one below and this one below fill the buffer from one above
// i mean the api function
pub fn handleFunc(
    method: Parse.Methods,
    path: []const u8,
    func: *const fn (w: *ResponseWriter, r: *Parse.HttpTemplate, allocator: std.mem.Allocator) anyerror!void,
) void {
    switch (method) {
        .GET => {
            var i: u32 = 0;
            while (true) : (i += 1) {
                if (Io.methGET.path[i] == null) {
                    Io.methGET.path[i] = path;
                    Io.methGET.func[i] = func;
                    std.debug.print("{d}: {s}\n", .{ i, Io.methGET.path[i].? });
                    break;
                }
            }
        },
        else => unreachable,
    }
}

test {
    std.testing.refAllDecls(@This());
}
