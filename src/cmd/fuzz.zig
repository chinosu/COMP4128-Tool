pub fn fuzz(alloc: mem.Allocator) !void {
    const cwd = fs.cwd();

    var main_cxx = try cxx.spawn(alloc, path.main_src, path.main_exe, .fast);
    var slow_cxx = try cxx.spawn(alloc, path.slow_src, path.slow_exe, .fast);
    try main_cxx.wait();
    defer cwd.deleteFile(path.main_exe) catch {};
    try slow_cxx.wait();
    defer cwd.deleteFile(path.slow_exe) catch {};

    var zig = Child.init(&.{ "zig", "build-lib", path.gen, "-dynamic" }, alloc);
    zig.stdin_behavior = .Close;
    zig.stdout_behavior = .Close;
    zig.stderr_behavior = .Pipe;

    try zig.spawn();
    const zig_err = try zig.stderr.?.readToEndAlloc(alloc, 1024);
    defer alloc.free(zig_err);

    if (zig_err.len != 0) {
        var lines = mem.tokenizeScalar(u8, zig_err, '\n');
        while (lines.next()) |line| log.warn("(zig) {s}", .{line});
    }

    switch ((try zig.wait()).Exited) {
        0 => log.info("compile success {s}", .{path.gen}),
        else => |c| return log.err("compile exit {d} for {s}", .{ c, path.gen }),
    }

    const gen_t = *const fn (*Random, *MList(u8)) bool;
    var gen_lib = try DynLib.open(path.genlib);
    const gen = gen_lib.lookup(gen_t, "gen") orelse return log.err("find `gen` fn from dylib fail", .{});
    log.info("found `gen` from dylib", .{});

    var meta_random = Random.DefaultPrng.init(@intCast(time.nanoTimestamp()));
    var random = meta_random.random();
    var in = try MList(u8).initCapacity(alloc, 64);
    defer in.deinit();
    if (!gen(&random, &in)) return log.err("gen fail", .{});
    log.info("generate {d} bytes", .{in.items.len});

    var proc = Child.init(&.{path.main_exe}, alloc);
    proc.stdin_behavior = .Pipe;
    proc.stdout_behavior = .Pipe;
    proc.stderr_behavior = .Pipe;
    var out = try List(u8).initCapacity(alloc, 128);
    defer out.deinit(alloc);
    var err = try List(u8).initCapacity(alloc, 128);
    defer err.deinit(alloc);
    try proc.spawn();
    try proc.stdin.?.writeAll(in.items);
    try proc.collectOutput(alloc, &out, &err, 2048);
    _ = try proc.wait();

    var slow_proc = Child.init(&.{path.slow_exe}, alloc);
    slow_proc.stdin_behavior = .Pipe;
    slow_proc.stdout_behavior = .Pipe;
    slow_proc.stderr_behavior = .Pipe;
    var slow_out = try List(u8).initCapacity(alloc, 128);
    defer slow_out.deinit(alloc);
    var slow_err = try List(u8).initCapacity(alloc, 128);
    defer slow_err.deinit(alloc);
    try slow_proc.spawn();
    try slow_proc.stdin.?.writeAll(in.items);
    try slow_proc.collectOutput(alloc, &slow_out, &slow_err, 2048);
    _ = try slow_proc.wait();
}

const cxx = @import("../cxx.zig");
const path = @import("../path.zig");

const List = @import("std").ArrayListUnmanaged;
const MList = @import("std").ArrayList;
const Child = @import("std").process.Child;
const log = @import("std").log;
const fs = @import("std").fs;
const mem = @import("std").mem;
const time = @import("std").time;
const DynLib = @import("std").DynLib;
const Random = @import("std").Random;
