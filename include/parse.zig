const std = @import("std");

// method path and version
// if some of this does not exist make it undefined so it's not going to effect when i
// combined all of them
const MPVer = struct {
    Method: [*:0]c_char,
    Path: [*:0]c_char,
    HTTPver: [*:0]c_char,
};
const DeviceInfo = struct {
    HostAddrAndPort: [*:0]c_char,
    UserAgent: [*:0]c_char,
};
const Acc = struct {
    Accept: [*:0]c_char,
    AcceptLanguange: [*:0]c_char,
    AcceptEncoding: [*:0]c_char,
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
