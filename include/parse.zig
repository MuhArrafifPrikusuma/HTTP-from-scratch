const std = @import("std");
const parser = error{
    unknownRequest,
    no_request,
    unknown_len,
};

pub fn parseHTTP(bytes: [*:0]const c_char, bytes_len: c_uint) parser!void {}
