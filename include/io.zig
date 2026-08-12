const std = @import("std");
const c = @cImport({
    @cInclude("../server/server.h");
});

fn readAll(fd: i32, allocator: std.mem.Allocator ) void {
    var total_read: usize = 0;
    var n: usize = 0;

    while (1) {
        n = c.read(fd, , __nbytes: usize)
    }
}

pub fn Reader(fd: c_int) void {
    const arena = std.heap.ArenaAllocator.init(std.heap.MemoryPool([]u8));
    const allocator = arena.allocator();
    defer arena.deinit();

    readAll(@as(i32, fd), allocator);


}
pub fn Writer(fd: c_int, max_size: usize) void {}
