pub fn main() !void {
    var debug_alloc = if (builtin.mode == .Debug) heap.DebugAllocator(.{ .safety = true }).init;
    defer if (builtin.mode == .Debug) assert(debug_alloc.deinit() == .ok);
    const alloc = if (builtin.mode == .Debug) debug_alloc.allocator() else heap.smp_allocator;

    var args = process.argsWithAllocator(alloc) catch unreachable;
    defer args.deinit();
    assert(args.skip());
    const c = args.next() orelse return log.err("enter a command", .{});

    if (eql(u8, c, "i") or eql(u8, c, "ini")) {
        const name = args.next() orelse return log.err("you must specify a name", .{});
        try ini(name);
    } else if (eql(u8, c, "t") or eql(u8, c, "test")) {
        try tests(alloc);
    } else if (eql(u8, c, "f") or eql(u8, c, "fuzz")) {
        try fuzz(alloc);
    } else if (eql(u8, c, "meta")) {
        @panic("todo");
    } else {
        const cxx = @import("cxx.zig");
        _ = try cxx.spawn(alloc, "a", "b", .debug);
        return log.err("unrecognized command '{s}'", .{c});
    }
}

const ini = @import("cmd/ini.zig").ini;
const tests = @import("cmd/tests.zig").tests;
const fuzz = @import("cmd/fuzz.zig").fuzz;

const assert = std.debug.assert;

const log = std.log;
const eql = std.mem.eql;
const process = std.process;
const heap = std.heap;

const std = @import("std");
const builtin = @import("builtin");
