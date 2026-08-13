// const std = @import("std");
// const root = @import("root");
//
// // method path and version
// const MPVer = struct {
//     Method: []u8,
//     Path: []u8,
//     HTTPver: []u8,
// };
// const DeviceInfo = struct {
//     HostAddrAndPort: []u8,
//     UserAgent: []u8,
// };
// const Acc = struct {
//     Accept: []u8,
//     AcceptLanguange: []u8,
//     AcceptEncoding: []u8,
// };
// // after we parsed it store it here
// const HttpTemplate = struct {
//     MPV: MPVer,
//     DInfo: DeviceInfo,
//     Accepts: Acc,
//     // take all of those and if undefined then deprecated else if it's filled we take those
//     fn buildString(self: HttpTemplate) []u8 {}
//     // convert the string that we just build into C compatible NULL terminated string
//     fn converToCstring(string: []u8, len: usize) root.CString {}
// };
//
// const ParserErr = error{
//     unknownRequest,
//     no_request,
//     unknown_len,
// };
//
// pub fn parseHTTP(bytes: root.CString, bytes_len: c_uint) ParserErr!void {
//     // make this heap allocated later on
// }
