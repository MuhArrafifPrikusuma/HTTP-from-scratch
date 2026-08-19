const lib = @import("common.zig");
const std = @import("std");

pub const Method: type = lib.Parse.Methods;
pub const Cstring: type = lib.Cstring;
const ResponseStatus = struct {
    // success
    OK: []const u8 = "200 OK",
    Created: []const u8 = "201 Created",
    // client error
    BadRequest: []const u8 = "400 Bad Request",
    NotFound: []const u8 = "404 Not Found",
    // server error
    InternalServerError: []const u8 = "500 Internal Server Error",
};

pub const ResponseWriter = struct {
    requestLine: lib.Parse.Line,

    pub fn WriterStatus(self: *@This(), status: ResponseStatus) !void {
        var buf: [256]u8 = undefined;
        const result = std.fmt.bufPrint(&buf, "HTTP/1.1 {s}\r\n", .{status});
        self.*.requestLine.status = result;
    }
};

pub fn handleFunc(
    method: lib.Parse.Methods,
    path: []const u8,
    func: *const fn (w: *ResponseWriter) void,
) void {
    switch (method) {
        .GET => {
            var i: u32 = 0;
            while (true) : (i += 1) {
                if (lib.Io.methGET.path[i] == null) {
                    lib.Io.methGET.path[i] = path;
                    lib.Io.methGET.func[i] = func;
                    std.debug.print("{d}: {s}\n", .{ i, lib.Io.methGET.path[i].? });
                    break;
                }
            }
        },
        else => unreachable,
    }
}
