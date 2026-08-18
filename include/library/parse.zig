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
    ContentLength: ?[]const u8 = null,
    ContentEncoding: ?[]const u8 = null,
    ContentLanguage: ?[]const u8 = null,
};
// after we parsed it store it here
pub const HttpTemplate = struct {
    routing: GeneralRouting = .{},
    client: ClientAgent = .{},
    auth: Auth = .{},
    payload: ContentPayload = .{},
    body: ?[]const u8 = null,
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
            .body = null,
            .client = .{},
            .auth = .{},
            .payload = .{},
            .routing = .{},
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
    FieldsNotFound,
    FailedToExtractContent,
    UnknownFieldName,
};

pub fn parseHTTP(bytesPtr: *anyopaque, io: *const std.Io) !*HttpTemplate {
    const bytes: *lib.ReadContext = @ptrCast(@alignCast(bytesPtr));
    defer bytes.deinit();

    const request = try HttpTemplate.init(std.heap.smp_allocator);

    _ = io;

    parser.splitPayload(bytes.readBuffer, request) catch |err| std.debug.print("{any}\n", .{err});

    return request;
}

fn splitPayload(bytes: []const u8, request: *HttpTemplate) !void {
    var iter = std.mem.splitSequence(u8, bytes, "\r\n");
    const allocator = request.arena.allocator();

    parser.getBody(bytes, "\r\n", request) catch |err| std.debug.print("body not found: {any}\n", .{err});
    var i: u16 = 0;
    while (true) : (i += 1) {
        if (iter.peek() == null) break;

        const current = iter.next() orelse return;
        if (i == 0) {
            request.routing.RequestLine = try allocator.dupe(u8, current);
            std.debug.print("[DEBUG]request line: {s}\n", .{request.routing.RequestLine.?});
            continue;
        }

        parser.determineHeader(current, request);
    }
    std.debug.print("[DEBUG]body: {s}\n[DEBUG]body length: {d}\n", .{ request.body.?, request.body.?.len });
}

const whatHeader = *const fn (slice: []const u8, request: *HttpTemplate) anyerror!void;

fn hostHandler(slice: []const u8, request: *HttpTemplate) !void {
    const host = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("failed to find host\n", .{});
        return;
    };

    const allocator = request.arena.allocator();
    request.routing.Host = try allocator.dupe(u8, host);

    std.debug.print("[DEBUG]Host: {s}\n", .{request.routing.Host.?});
}
fn connectionHandler(slice: []const u8, request: *HttpTemplate) !void {
    const conType = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find connection type", .{});
        return;
    };
    const allocator = request.arena.allocator();
    request.routing.Connection = try allocator.dupe(u8, conType);

    std.debug.print("[DEBUG]connection: {s}\n", .{request.routing.Connection.?});
}
/// NOTE: not needed for now, maybe later
fn upgradeHandler(slice: []const u8, request: *HttpTemplate) !void {
    std.debug.print("[DEBUG]upgrade: {s}\n", .{slice});
    _ = request;
}
fn agentHandler(slice: []const u8, request: *HttpTemplate) !void {
    const agent = parser.getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find user agent", .{});
        return;
    };

    const allocator = request.arena.allocator();

    request.client.UserAgent = try allocator.dupe(u8, agent);
    std.debug.print("[DEBUG]agent: {s}\n", .{request.client.UserAgent.?});
}

// NOTE: finish this later
fn acceptHandler(slice: []const u8, request: *HttpTemplate) !void {
    const possibleValues = [_][]const u8{
        "Accept",
        "Language",
        "Encoding",
        "Charset",
    };

    const index = parser.findExtension(slice, &possibleValues, "-", 2, true) catch |err|
        switch (err) {
            ParserErr.FieldsNotFound, ParserErr.UnknownFieldName => {
                std.debug.print("{any} while parsing accept\n", .{err});
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

    // NOTE: delete this print statements later
    switch (index) {
        0 => {
            request.client.Accept = try allocator.dupe(u8, accept);
            std.debug.print("[DEBUG]Accept: {s}\n", .{request.client.Accept.?});
        },
        1 => {
            request.client.AcceptLanguage = try allocator.dupe(u8, accept);
            std.debug.print("[DEBUG]Accept-Language: {s}\n", .{request.client.AcceptLanguage.?});
        },
        2 => {
            request.client.AcceptEncoding = try allocator.dupe(u8, accept);
            std.debug.print("[DEBUG]Accept-Encoding: {s}\n", .{request.client.AcceptEncoding.?});
        },
        3 => {
            request.client.AcceptCharset = try allocator.dupe(u8, accept);
            std.debug.print("[DEBUG]Accept-Charset: {s}\n", .{request.client.AcceptCharset.?});
        },
        else => unreachable,
    }
}
/// unused for now
fn DNTHandler(slice: []const u8, request: *HttpTemplate) !void {
    _ = request;
    std.debug.print("DNT: {s}\n", .{slice});
}
/// unused for now
fn authHandler(slice: []const u8, request: *HttpTemplate) !void {
    _ = request;
    std.debug.print("auth: {s}\n", .{slice});
}
// unused for now
fn cookieHandler(slice: []const u8, request: *HttpTemplate) !void {
    _ = request;
    std.debug.print("cookie: {s}\n", .{slice});
}
fn contentHandler(slice: []const u8, request: *HttpTemplate) !void {
    const possibleContent = [_][]const u8{
        "Type",
        "Encoding",
        "Length",
        "Language",
    };

    const index = parser.findExtension(slice, &possibleContent, "-", 2, false) catch |err|
        switch (err) {
            ParserErr.FieldsNotFound, ParserErr.UnknownFieldName => {
                std.debug.print("{any} while parsing content\n", .{err});
                return;
            },
            else => unreachable,
        };

    const content = getContent(slice) catch |err| {
        std.debug.print("{any}\n", .{err});
        return;
    } orelse {
        std.debug.print("cannot find Accept fields\n", .{});
        return;
    };

    const allocator = request.arena.allocator();

    switch (index) {
        0 => {
            request.payload.ContentType = try allocator.dupe(u8, content);
            std.debug.print("[DEBUG]Content-Type: {s}\n", .{request.payload.ContentType.?});
        },
        1 => {
            request.payload.ContentEncoding = try allocator.dupe(u8, content);
            std.debug.print("[DEBUG]Content-Encoding: {s}\n", .{request.payload.ContentEncoding.?});
        },
        2 => {
            request.payload.ContentLength = try allocator.dupe(u8, content);
            std.debug.print("[DEBUG]Content-Length: {s}\n", .{request.payload.ContentLength.?});
        },
        3 => {
            request.payload.ContentLanguage = try allocator.dupe(u8, content);
            std.debug.print("[DEBUG]Content-Language: {s}\n", .{request.payload.ContentLanguage.?});
        },
        else => unreachable,
    }
}
/// unused for now
fn secHandler(slice: []const u8, request: *HttpTemplate) !void {
    _ = request;
    std.debug.print("[DEBUG]sec: {s}\n", .{slice});
}

fn determineHeader(slice: []const u8, request: *HttpTemplate) void {
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
            jump(slice, request) catch |err| std.debug.print("determineHeader: {any}\n", .{err});
            break;
        }
    }
}

fn getBody(slice: []const u8, delimiter: []const u8, request: *HttpTemplate) !void {
    var iter = std.mem.splitSequence(u8, slice, delimiter);

    var tmp: []const u8 = undefined;
    while (iter.peek() != null) {
        tmp = iter.next().?;
    }

    const allocator = request.arena.allocator();
    request.body = try allocator.dupe(u8, tmp);
}

/// return the index that matches your field extension
/// you must declare the default field with no extension on index 0
fn findExtension(slice: []const u8, comptime possibleValues: []const []const u8, comptime delimiter: []const u8, searchDepth: u8, comptime zeroAsFallback: bool) !i8 {
    var iter = std.mem.splitSequence(u8, slice, ":");
    const mainField = iter.next() orelse return ParserErr.FieldsNotFound;

    var iterExt = std.mem.splitSequence(u8, mainField, delimiter);
    var extension: []const u8 = undefined;
    var searchThrough = searchDepth;

    while (searchThrough > 0) : (searchThrough -= 1) {
        extension = iterExt.next() orelse return ParserErr.FieldsNotFound;

        if (searchThrough == searchDepth) {
            if (zeroAsFallback) {
                if (!std.ascii.eqlIgnoreCase(extension, possibleValues[0])) return ParserErr.UnknownFieldName;
                if (iterExt.peek() == null) return 0;
            }
        }
    }

    for (possibleValues, 0..) |value, i| {
        if (std.ascii.eqlIgnoreCase(value, extension))
            return @as(i8, @intCast(i));
    }
    if (zeroAsFallback)
        return 0;
    return ParserErr.UnknownFieldName;
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
