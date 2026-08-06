const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

// it will throw failed command because it's
// getting hijacked by the new socket we opened in this function but it's completely fine
// as long as we use --summary all
test "server C get_listener_socket" {
    const listen_fd = c.get_listener_socket("0");
    try std.testing.expect(listen_fd != -1);
}
