const std = @import("std");

pub const Io = @import("io.zig");
pub const Parse = @import("parse.zig");

pub const Cstring: type = [*:0]const u8;

pub const ReadContext = struct {
    arena: std.heap.ArenaAllocator,
    buffer: [4096]u8,
    readBuffer: []u8,

    pub fn init(backing_allocator: std.mem.Allocator) !*ReadContext {
        var arena = std.heap.ArenaAllocator.init(backing_allocator);
        errdefer arena.deinit();

        const allocator = arena.allocator();
        const self = try allocator.create(ReadContext);

        self.* = .{
            .arena = arena,
            .buffer = undefined,
            .readBuffer = undefined,
        };

        return self;
    }

    pub fn deinit(self: *ReadContext) void {
        const arena = self.arena;
        arena.deinit();
    }
};

export fn eReader(fd: c_int, from_addr: Cstring, ioptr: *anyopaque) ?*anyopaque {
    const ptr = Io.Reader(fd, from_addr, ioptr) catch return null;
    return ptr;
}

test {
    std.testing.refAllDecls(@This());
}
