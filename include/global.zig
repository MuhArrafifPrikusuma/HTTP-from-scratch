const std = @import("std");
//
// extern var environ: [*:null]?[*:0]const u8;
//
// pub export fn read_env_from_libc() void {
//     var i: usize = 0;
//     while (environ[i]) |env_ptr| : (i += 1) {
//         const env_str = std.mem.span(env_ptr);
//
//         if (std.mem.indexOfScalar(u8, env_str, '=')) |pos| {
//             const key = env_str[0..pos];
//             const value = env_str[pos + 1 ..];
//             std.debug.print("{s}\n", .{key});
//             std.debug.print("{s}\n", .{value});
//
//             if (std.mem.eql(u8, key, "PATH")) {
//                 std.log.info("Found path: {s}\n", .{value});
//                 break;
//             }
//         }
//     }
// }
pub fn main(init: std.process.Init) !void {
    var gpa = std.heap.DebugAllocator(.{}){};
    const allocator = gpa.allocator();
    defer _ = gpa.deinit();

    const matrix = init.environ_map.values();

    var list: std.ArrayList(u8) = .empty;
    defer list.deinit(allocator);

    for (matrix) |row| {
        for (row) |val| {
            try list.print(allocator, "{c}", .{val});
        }
        try list.print(allocator, "\n", .{});
    }
    const clean_string = list.items;

    std.debug.print("{s}\n", .{clean_string});
}
