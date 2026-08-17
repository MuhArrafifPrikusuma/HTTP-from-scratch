const std = @import("std");
const lib = @import("common.zig");
const parser = @This();

// method path and version
const GeneralRouting = struct {
    RequestLine: ?[]const u8 = null,
    Host: ?[]const u8 = null,
    Connection: ?[]const u8 = null,
    Upgrade: ?[]const u8 = null,
};
const ClientAgent = struct {
    UserAgent: ?[]const u8 = null,
    Accept: ?[]const u8 = null,
    AcceptLanguage: ?[]const u8 = null,
    AcceptEncoding: ?[]const u8 = null,
    AcceptCharset: ?[]const u8 = null,
    DNT: ?[]const u8 = null, // <- do not track
};
const Auth = struct {
    Authorization: ?[]const u8 = null,
    Cookie: ?[]const u8 = null,
    ProxyAuthorization: ?[]const u8 = null,
};
const ContentPayload = struct {
    ContentType: ?[]const u8 = null,
    ContentLenght: ?[]const u8 = null,
    ContentEncoding: ?[]const u8 = null,
    ContentLanguage: ?[]const u8 = null,
};
// after we parsed it store it here
const HttpTemplate = struct {
    routing: GeneralRouting = undefined,
    client: ClientAgent = undefined,
    auth: Auth = undefined,
    payload: ContentPayload = undefined,
    arena: std.heap.ArenaAllocator,

    pub fn parseRequestLine(self: *HttpTemplate) void {
        _ = self;
    }

    /// allocate to arena pointer
    pub fn init(child_allocator: std.mem.Allocator) !*HttpTemplate {
        var arena = std.heap.ArenaAllocator.init(child_allocator);
        errdefer arena.deinit();

        const allocator = arena.allocator();

        const self = try allocator.create(HttpTemplate);

        self.* = .{
            .arena = arena,
            .client = undefined,
            .auth = undefined,
            .payload = undefined,
            .routing = undefined,
        };
        return self;
    }

    pub fn deinit(self: *HttpTemplate) void {
        self.arena.deinit();
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
    UnknownFieldName,
};

pub fn parseHTTP(bytesPtr: *anyopaque, io: *const std.Io) !void {
    const bytes: *lib.ReadContext = @ptrCast(@alignCast(bytesPtr));
    defer bytes.destroy();

    const request = try HttpTemplate.init(std.heap.smp_allocator);

    _ = io;

    parser.splitPayload(&bytes.readBuffer, request);
}

fn splitPayload(bytes: *const []u8, request: *HttpTemplate) void {
    var iter = std.mem.splitSequence(u8, bytes.*, "\r\n");
    const allocator = request.arena.allocator();

    var i: u16 = 0;
    while (true) : (i += 1) {
        if (iter.peek() == null) break;

        const current = iter.next() orelse return;
        if (i == 0) {
            request.routing.RequestLine = allocator.dupe(u8, current) catch unreachable;
            std.debug.print("this is request line: {s}\n", .{request.routing.RequestLine.?});
            continue;
        }

        determineHeader(current, request) catch unreachable;
    }
}

const whatHeader = *const fn (noalias slice: []const u8, request: *HttpTemplate) void;

fn hostHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    const host = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("failed to find host\n", .{});
        return;
    };

    const allocator = request.arena.allocator();
    request.routing.Host = allocator.dupe(u8, host) catch unreachable;

    std.debug.print("extracted: {s}\n", .{request.routing.Host.?});
}
fn connectionHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    const conType = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find connection type", .{});
        return;
    };
    const allocator = request.arena.allocator();
    request.routing.Connection = allocator.dupe(u8, conType) catch unreachable;

    std.debug.print("connection: {s}\n", .{request.routing.Connection.?});
}
/// NOTE: not needed for now, maybe later
fn upgradeHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    std.debug.print("upgrade: {s}\n", .{slice});
    _ = request;
}
fn agentHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    const agent = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find user agent", .{});
        return;
    };

    const allocator = request.arena.allocator();

    request.client.UserAgent = allocator.dupe(u8, agent) catch unreachable;
    std.debug.print("agent: {s}\n", .{request.client.UserAgent.?});
}

// NOTE: finish this later
fn acceptHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    const possibleValues = [_][]const u8{
        "Accept",
        "Languange",
        "Encoding",
        "Charset",
    };

    const index = parser.findExtension(slice, &possibleValues, "-", 2) catch |err|
        switch (err) {
            ParserErr.FieldsNotFound, ParserErr.UnknownFieldName => {
                std.debug.print("{any} in acceptHandler\n", .{err});
                return;
            },
            else => unreachable,
        };

    const accept = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find Accept fields\n", .{});
        return;
    };

    const allocator = request.arena.allocator();

    switch (index) {
        0 => request.client.Accept = allocator.dupe(u8, accept) catch unreachable,
        1 => request.client.AcceptLanguage = allocator.dupe(u8, accept) catch unreachable,
        2 => request.client.AcceptEncoding = allocator.dupe(u8, accept) catch unreachable,
        3 => request.client.AcceptCharset = allocator.dupe(u8, accept) catch unreachable,
        else => unreachable,
    }
}
fn DNTHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("DNT: {s}\n", .{slice});
}
fn authHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("auth: {s}\n", .{slice});
}
fn cookieHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("cookie: {s}\n", .{slice});
}
fn contentHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("content: {s}\n", .{slice});
}
fn secHandler(noalias slice: []const u8, request: *HttpTemplate) void {
    _ = request;
    std.debug.print("sec: {s}\n", .{slice});
}

fn determineHeader(noalias slice: []const u8, request: *HttpTemplate) error{UnknownFields}!void {
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

/// return the index that matches your field extension
/// you must declare the default field with no extension on index 0
fn findExtension(slice: []const u8, possibleValues: []const []const u8, delimiter: []const u8, searchDepth: u8) ParserErr!i8 {
    var iter = std.mem.splitSequence(u8, slice, ":");
    const mainField = iter.next() orelse return ParserErr.FieldsNotFound;

    var iterExt = std.mem.splitSequence(u8, mainField, delimiter);
    var extension: []const u8 = undefined;
    var searchThrough = searchDepth;

    while (searchThrough > 0) : (searchThrough -= 1) {
        extension = iterExt.next() orelse unreachable;
        if (searchThrough == searchDepth)
            if (!std.ascii.eqlIgnoreCase(extension, possibleValues[0])) return ParserErr.UnknownFieldName;
    }

    std.debug.print("this si the extension: {s}\n", .{extension});
    for (possibleValues, 0..) |value, i| {
        if (std.ascii.eqlIgnoreCase(value, extension))
            return @as(i8, @intCast(i));
    }
    return 0;
}

/// get fields and strip the content down from slice
fn getFields(slice: []const u8) ParserErr![]const u8 {
    var iter = std.mem.splitSequence(u8, slice, ":");
    if (iter.peek() != null) iter = std.mem.splitSequence(u8, iter.peek().?, "-");

    const val = iter.next() orelse return ParserErr.FieldsNotFound;

    return val;
}

/// strip the field and take the content
fn getContent(slice: []const u8) ParserErr!?[]const u8 {
    var iterField = std.mem.splitSequence(u8, slice, ": ");
    if (iterField.next() == null) return null;

    var iterContent = std.mem.splitSequence(u8, iterField.next().?, "\r\n");
    if (iterContent.peek().?.len == 0) return null;
    if (iterContent.peek() == null) return ParserErr.FailedToExtractContent;

    const content = iterContent.next() orelse null;

    return content;
}
