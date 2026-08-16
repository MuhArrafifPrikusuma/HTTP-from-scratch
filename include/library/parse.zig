const std = @import("std");
const lib = @import("common.zig");

// method path and version
const GeneralRouting = struct {
    RequestLine: ?[]const u8,
    Host: ?[]const u8,
    Connection: ?[]const u8,
    Upgrade: ?[]const u8,
};
const ClientAgent = struct {
    UserAgent: ?[]const u8,
    Accept: ?[]const u8,
    AcceptLanguage: ?[]const u8,
    AcceptEncoding: ?[]const u8,
    AcceptCharset: ?[]const u8,
    DNT: ?[]const u8, // <- do not track
};
const Auth = struct {
    Authorization: ?[]const u8,
    Cookie: ?[]const u8,
    ProxyAuthorization: ?[]const u8,
};
const ContentPayload = struct {
    ContentType: ?[]const u8,
    ContentLenght: ?[]const u8,
    ContentEncoding: ?[]const u8,
    ContentLanguage: ?[]const u8,
};
// after we parsed it store it here
const HttpTemplate = struct {
    routing: GeneralRouting,
    client: ClientAgent,
    auth: Auth,
    payload: ContentPayload,
    arena: std.heap.ArenaAllocator,

    pub fn parseRequestLine(self: *HttpTemplate) void {
        _ = self;
    }

    pub fn create(backing_allocator: std.mem.Allocator) !*HttpTemplate {
        var arena = std.heap.ArenaAllocator.init(backing_allocator);
        errdefer arena.deinit();

        const allocator = arena.allocator();
        const self = try allocator.create(HttpTemplate);

        self.* = .{
            .routing = undefined,
            .client = undefined,
            .auth = undefined,
            .payload = undefined,
            .arena = arena,
        };

        return self;
    }

    pub fn destroy(self: *HttpTemplate) void {
        const arena = self.arena;
        arena.deinit();
    }
    // // take all of those and if undefined then deprecated else if it's filled we take those
    // fn buildString(self: *HttpTemplate) []u8 {}
    // // convert the string that we just build into C compatible NULL terminated string
    // fn converToCstring(string: []u8, len: usize) root.CString {}
};

const ParserErr = error{
    unknownRequest,
    no_request,
    unknown_len,
    invalid_request,
};

pub fn parseHTTP(bytesPtr: *anyopaque, io: *const std.Io) ParserErr!void {
    const bytes: *lib.ReadContext = @ptrCast(@alignCast(bytesPtr));
    defer bytes.destroy();

    const request = HttpTemplate.create(std.heap.smp_allocator) catch unreachable;
    _ = io;

    _ = splitPayload(&bytes.readBuffer, request);
}

fn splitPayload(bytes: *const []u8, request: *HttpTemplate) void {
    var iter = std.mem.splitSequence(u8, bytes.*, "\r\n");

    var i: u16 = 0;
    while (true) : (i += 1) {
        if (iter.peek() == null) break;

        const current = iter.next();
        if (i == 0) {
            request.*.routing.RequestLine = current.?;
            continue;
        }

        std.debug.print("contain:{s}\nlen:{d}\n", .{ current.?, current.?.len });
        std.debug.print("{any}\n", .{@TypeOf(current.?)});
        // _ = determineHeader(&current.?) catch unreachable;
    }
}

const knownHeader = [_]u8{
    "Host",
    "Connection",
    "Upgrade",
    "User-Agent",
    "Accept",
    "DNT",
    "Authorization",
    "Cookie",
    "Content",
    "Sec",
};

// fn determineHeader(slice: *const []const u8) error{UnknownFields}!*HttpTemplate {
//     _ = slice;
// }
