const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

// it will throw failed command because it's
// getting hijacked by the new socket we opened in this function but it's completely fine
// as long as we use --summary all
test "server C get_listener_socket" {
    const listen_fd = c.get_listener_socket("0");
    try std.testing.expect(listen_fd >= 0);

    const epfd = c.init_epoll_fd();
    try std.testing.expect(epfd != -1);

    const connection_fd = c.accept_incoming_connection(listen_fd);
    try std.testing.expect(connection_fd == -1);

    var event_struct: [1024]c.epoll_event = undefined;

    const num_of_events = c.epoll_add_events(2, epfd, &event_struct);
    try std.testing.expect(num_of_events != -1);
}
