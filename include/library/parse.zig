const std = @import("std");
const lib = @import("common.zig");
const parser = @This();

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
    FieldsNotFound,
    FailedToExtractContent,
};

pub fn parseHTTP(bytesPtr: *anyopaque, io: *const std.Io) ParserErr!void {
    const bytes: *lib.ReadContext = @ptrCast(@alignCast(bytesPtr));
    defer bytes.destroy();

    const request = parser.HttpTemplate.create(std.heap.smp_allocator) catch |err| {
        std.debug.print("Failed to allocate memory for HttpTemplate: {any}\n", .{err});
        return;
    };
    _ = io;

    parser.splitPayload(&bytes.readBuffer, request);
}

fn splitPayload(bytes: *const []u8, request: *HttpTemplate) void {
    var iter = std.mem.splitSequence(u8, bytes.*, "\r\n");

    var i: u16 = 0;
    while (true) : (i += 1) {
        if (iter.peek() == null) break;

        const current = iter.next();
        if (i == 0) {
            request.*.routing.RequestLine = current.?;
            std.debug.print("this is request line: {s}\n", .{request.*.routing.RequestLine.?});
            continue;
        }

        determineHeader(&current.?, request) catch unreachable;
    }
}

const knownHeader = [_][]const u8{
    "Host",
    "Connection",
    "Upgrade",
    "User",
    "Accept",
    "DNT",
    "Authorization",
    "Cookie",
    "Content",
    "Sec",
};

const whatHeader = *const fn (slice: *const []const u8, request: *HttpTemplate) void;

fn hostHandler(slice: *const []const u8, request: *HttpTemplate) void {
    std.debug.print("host: {s}\n", .{slice.*});

    const host = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    };

    request.*.routing.Host = host orelse null;
}
fn connectionHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("connection: {s}\n", .{slice.*});
}
fn upgradeHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("upgrade: {s}\n", .{slice.*});
}
fn agentHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("agent: {s}\n", .{slice.*});
}
fn acceptHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("accpet: {s}\n", .{slice.*});
}
fn DNTHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("DNT: {s}\n", .{slice.*});
}
fn authHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("auth: {s}\n", .{slice.*});
}
fn cookieHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("cookie: {s}\n", .{slice.*});
}
fn contentHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("content: {s}\n", .{slice.*});
}
fn secHandler(slice: *const []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("sec: {s}\n", .{slice.*});
}

fn determineHeader(slice: *const []const u8, request: *HttpTemplate) error{UnknownFields}!void {
    const jumpTable = [_]whatHeader{
        &hostHandler,
        &connectionHandler,
        &upgradeHandler,
        &agentHandler,
        &acceptHandler,
        &DNTHandler,
        &authHandler,
        &cookieHandler,
        &contentHandler,
        &secHandler,
    };

    const field = parser.getFields(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    };
    for (jumpTable, knownHeader) |jump, header| {
        if (std.ascii.eqlIgnoreCase(header, field)) {
            jump(slice, request);
            break;
        }
    }
}

// get fields from the slice
fn getFields(slice: *const []const u8) ParserErr![]const u8 {
    var iter = std.mem.splitSequence(u8, slice.*, ":");
    var tmp = std.mem.splitSequence(u8, iter.peek().?, "-");
    if (tmp.peek() != null) iter = tmp;

    const val = iter.next();
    if (val == null)
        return ParserErr.FieldsNotFound;
    return val.?;
}

fn getContent(slice: *const []const u8) ParserErr!?[]const u8 {
    var iterField = std.mem.splitSequence(u8, slice.*, ": ");
    if (iterField.next() == null) return null;

    var iterContent = std.mem.splitSequence(u8, iterField.next().?, "\r\n");
    if (iterContent.peek().?.len == 0) return null;
    if (iterContent.peek() == null) return ParserErr.FailedToExtractContent;

    std.debug.print("extracted content: {s}\n", .{iterContent.next().?});

    return iterContent.peek();
}
