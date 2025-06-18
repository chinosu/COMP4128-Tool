pub fn main() !void {
    var debug_alloc = if (builtin.mode == .Debug) heap.DebugAllocator(.{ .safety = true }).init;
    defer if (builtin.mode == .Debug) assert(debug_alloc.deinit() == .ok);
    const alloc = if (builtin.mode == .Debug) debug_alloc.allocator() else heap.smp_allocator;

    var args = process.argsWithAllocator(alloc) catch unreachable;
    defer args.deinit();
    assert(args.skip());
    const c = args.next() orelse return log.err("enter a command", .{});

    if (mem.eql(u8, c, "ini")) {
        const name = args.next() orelse return log.err("you must specify a name", .{});
        try cmd.ini(name);
    } else if (mem.eql(u8, c, "t") or mem.eql(u8, c, "test")) {
        try cmd.tests(alloc);
    } else if (mem.eql(u8, c, "f") or mem.eql(u8, c, "fuzz")) {
        try cmd.fuzz(alloc);
    } else {
        return log.err("unrecognized command '{s}'", .{c});
    }
}

const cmd = @import("cmd.zig");

const assert = std.debug.assert;

const log = std.log;
const mem = std.mem;
const process = std.process;
const heap = std.heap;

const std = @import("std");
const builtin = @import("builtin");
