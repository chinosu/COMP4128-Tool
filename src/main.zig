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
    } else if (eql(u8, c, "c") or eql(u8, c, "code")) {
        _ = try run(.{
            .allocator = alloc,
            .argv = &.{ "code", path.main_src, path.tests },
            .expand_arg0 = .expand,
        });
    } else {
        return log.err("unrecognized command '{s}'", .{c});
    }
}

const ini = @import("cmd/ini.zig").ini;
const tests = @import("cmd/tests.zig").tests;
const fuzz = @import("cmd/fuzz.zig").fuzz;
const path = @import("path.zig");

const assert = @import("std").debug.assert;
const run = @import("std").process.Child.run;

const log = @import("std").log;
const eql = @import("std").mem.eql;
const process = @import("std").process;
const heap = @import("std").heap;

const builtin = @import("builtin");
