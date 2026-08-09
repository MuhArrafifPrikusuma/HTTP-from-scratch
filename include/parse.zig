const std = @import("std");

// method path and version
// if some of this does not exist make it undefined so it's not going to effect when i
// combined all of them
const Cstring: type = [*:0]c_char;
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
    // take all of those if undefined then deprecated else if it's filled we take those
    fn buildString() !Cstring {}
};

const ParserErr = error{
    unknownRequest,
    no_request,
    unknown_len,
};
pub fn parseHTTP(bytes: [*:0]const c_char, bytes_len: c_uint) ParserErr!void {
    // make this heap allocated later on
    const bytes_safe = @as([bytes_len]u8, bytes);
}
