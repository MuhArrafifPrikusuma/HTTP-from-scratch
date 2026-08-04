const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

var fd: c_int = 0;
// it will throw failed command because it's
// getting hijacked by the new socket we opened in this function but it's completely fine
// as long as we use --summary all
test "(server C) get_listener_socket" {
    fd = c.get_listener_socket("0");

    try std.testing.expect(fd >= 0);
}

// NOTE: create mock server later to test an actual client connection too
test "(Server C) accept_incoming_connection" {
    const connection_fd = c.accept_incoming_connection(fd);

    try std.testing.expect(connection_fd == -1);
}
