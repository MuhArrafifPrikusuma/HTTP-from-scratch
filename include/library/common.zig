const std = @import("std");

pub const Io = @import("io.zig");
pub const Parse = @import("parse.zig");

pub const Cstring: type = [*:0]const u8;

pub const ReadContext = struct {
    arena: std.heap.ArenaAllocator,
    read_buf: [4096]u8,

    pub fn startContext(backing_allocator: std.mem.Allocator) !*ReadContext {
        const returnContext = try backing_allocator.create(ReadContext);

        returnContext.arena = std.heap.ArenaAllocator.init(backing_allocator);
        @memset(&returnContext.read_buf, 0);

        return returnContext;
    }
    pub fn explodeContext(self: *ReadContext) !void {
        self.arena.deinit();
    }
};

export fn eReader(fd: c_int, from_addr: Cstring) *anyopaque {
    return Io.Reader(fd, from_addr);
}

test {
    std.testing.refAllDecls(@This());
}
