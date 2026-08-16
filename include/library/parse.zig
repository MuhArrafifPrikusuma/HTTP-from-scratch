const std = @import("std");
const lib = @import("common.zig");

// method path and version
const MPVer = struct {
    Method: []u8,
    Path: []u8,
    HTTPver: []u8,
};
const DeviceInfo = struct {
    HostAddrAndPort: []u8,
    UserAgent: []u8,
};
const Acc = struct {
    Accept: []u8,
    AcceptLanguange: []u8,
    AcceptEncoding: []u8,
};
// after we parsed it store it here
const HttpTemplate = struct {
    MPV: MPVer,
    DInfo: DeviceInfo,
    Accepts: Acc,
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
    const bytes: lib.ReadContext = @ptrCast(@alignCast(bytesPtr));
    defer bytes.destroy();
    _ = io;
}

fn splitPayload(bytes: *lib.ReadContext) !*HttpTemplate {}
