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

const ResponseStatus = struct {
    // success
    OK: []const u8 = "200 OK",
    Created: []const u8 = "201 Created",
    // client error
    BadRequest: []const u8 = "400 Bad Request",
    NotFound: []const u8 = "404 Not Found",
    // server error
    InternalServerError: []const u8 = "500 Internal Server Error",
};

pub const ResponseWriter = struct {
    requestLine: Parse.Line = .{},
    Http: Parse.HttpTemplate = .{},
    body: []const u8 = 0,

    pub fn WriterStatus(self: *@This(), status: ResponseStatus) !void {
        var buf: [256]u8 = undefined;
        const result = std.fmt.bufPrint(&buf, "HTTP/1.1 {s}\r\n", .{status});
        self.requestLine.status = result;
    }

    pub fn WriteContentType(self: *@This(), ContentType: []const u8) !void {
        var buf: [256]u8 = undefined;
        const result = std.fmt.bufPrint(&buf, "Content-Type: {s}", .{ContentType});
        self.Http.payload.ContentType = result;
    }

    pub fn WriteAll(self: *@This()) !void {
        const allocator = std.heap.smp_allocator;
        var response: std.ArrayList(u8) = .empty;
        defer response.deinit(allocator);

        // make header
        if (self.requestLine.status != null)
            try response.appendSlice(allocator, self.requestLine.status.?);
        if (self.Http.date != null)
            try response.appendSlice(allocator, self.Http.date.?);
    }
};

pub fn handleFunc(
    method: Parse.Methods,
    path: []const u8,
    func: *const fn (w: *ResponseWriter, r: *Parse.HttpTemplate) void,
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
