export fn gen(_: *void, random: *std.Random, bytes: *std.ArrayList(u8)) bool {
    var buf: [@max(20, 512)]u8 = undefined;

    bytes.appendSlice(bufPrint(&buf, "{d} hi :3", .{random.int(c_longlong)}) catch return false) catch return false;

    return true;
}

const std = @import("std");
const bufPrint = std.fmt.bufPrint;
