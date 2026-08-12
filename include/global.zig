const std = @import("std");

extern var environ: [*:null]?[*:0]const u8;

pub export fn read_env_from_libc() void {
    var i: usize = 0;
    while (environ[i]) |env_ptr| : (i += 1) {
        const env_str = std.mem.span(env_ptr);

        if (std.mem.indexOfScalar(u8, env_str, "=")) |pos| {
            const key = env_str[0..pos];
            const value = env_str[pos + 1 ..];

            if (std.mem.eql(u8, key, "PATH")) {
                std.log.info("Found path: {s}\n", .{value});
                break;
            }
        }
    }
}
