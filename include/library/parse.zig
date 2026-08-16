const std = @import("std");
const lib = @import("common.zig");

// method path and version
const GeneralRouting = struct {
    RequestLine: ?[]u8,
    Host: ?[]u8,
    Connection: ?[]u8,
    Upgrade: ?[]u8,
};
const ClientAgent = struct {
    UserAgent: ?[]u8,
    Accept: ?[]u8,
    AcceptLanguage: ?[]u8,
    AcceptEncoding: ?[]u8,
    AcceptCharset: ?[]u8,
    DNT: ?[]u8, // <- do not track
};
const Auth = struct {
    Authorization: ?[]u8,
    Cookie: ?[]u8,
    ProxyAuthorization: ?[]u8,
};
const ContentPayload = struct {
    ContentType: ?[]u8,
    ContentLenght: ?[]u8,
    ContentEncoding: ?[]u8,
    ContentLanguage: ?[]u8,
};
// after we parsed it store it here
const HttpTemplate = struct {
    routing: GeneralRouting,
    client: ClientAgent,
    auth: Auth,
    payload: ContentPayload,
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
    _ = io;

    _ = splitPayload(&bytes.read_buf);
}

fn splitPayload(bytes: []u8) ?*HttpTemplate {
    const request: ?*HttpTemplate = undefined;
    var iter = std.mem.splitSequence(u8, bytes, "\r\n");

    var i: u32 = 0;
    while (true) : (i += 1) {
        const current = iter.next();
        if (current.?.len == 0) break;
        std.debug.print("{d}:{s}\nlen:{d}\n", .{ i, current.?, current.?.len });
    }
    return request;
}

fn determineHeader() !void {}
