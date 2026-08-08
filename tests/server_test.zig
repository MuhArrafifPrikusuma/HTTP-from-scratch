const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

// it will throw failed command because it's
// getting hijacked by the new socket we opened in this function but it's completely fine
// as long as we use --summary all
test "server C get_listener_socket" {
    const tc = struct {
        port: [*:0]const u8,
        expect: bool,
    };

    const tcs = [_]tc{
        // NOTE: activate this later and use child process to test it
        // .{
        //     .port = "99999999",
        //     .expect = false,
        // },
        .{
            .port = "0",
            .expect = true,
        },
        .{
            .port = "0",
            .expect = true,
        },
    };

    for (tcs) |val| {
        const is_success: bool = (c.get_listener_socket(val.port) != -1);
        try std.testing.expect(is_success == val.expect);
    }
}
