pub fn main() !void {
    var debug_alloc = if (builtin.mode == .Debug) heap.DebugAllocator(.{ .safety = true }).init;
    defer if (builtin.mode == .Debug) assert(debug_alloc.deinit() == .ok);
    const alloc = if (builtin.mode == .Debug) debug_alloc.allocator() else heap.smp_allocator;

    var args = process.argsWithAllocator(alloc) catch unreachable;
    defer args.deinit();
    assert(args.skip());
    const command = args.next() orelse return log.err("enter a command", .{});

    if (mem.eql(u8, command, "ini")) {
        const name = args.next() orelse return log.err("you must specify a name", .{});
        try cmd.ini(name);
    } else if (mem.eql(u8, command, "t") or mem.eql(u8, command, "test")) {
        try cmd.tests(alloc);
    } else if (mem.eql(u8, command, "f") or mem.eql(u8, command, "fuzz")) {
        try cmd.fuzz(alloc);
    } else {
        return log.err("unrecognized command '{s}'", .{cmd});
    }
}

const cmd = @import("cmd.zig");

const List = std.ArrayListUnmanaged;
const MList = std.ArrayList;
const assert = std.debug.assert;

const log = std.log;
const mem = std.mem;
const process = std.process;
const heap = std.heap;
const Random = std.Random;
const time = std.time;
const fs = std.fs;
const fmt = std.fmt;
const DynLib = std.DynLib;

const std = @import("std");
const builtin = @import("builtin");
