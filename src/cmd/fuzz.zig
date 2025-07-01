pub fn fuzz(alloc: mem.Allocator) !void {
    const cwd = fs.cwd();

    var main_cxx = try cxx.spawn(alloc, path.main_src, path.main_exe, .fast);
    var slow_cxx = try cxx.spawn(alloc, path.slow_src, path.slow_exe, .fast);
    var gen_zigc = try zigc.spawn(alloc, path.gen);
    try cxx.wait(alloc, &main_cxx, path.main_src);
    defer cwd.deleteFile(path.main_exe) catch {};
    try cxx.wait(alloc, &slow_cxx, path.main_src);
    defer cwd.deleteFile(path.slow_exe) catch {};
    try zigc.wait(alloc, &gen_zigc, path.gen);
    defer cwd.deleteFile(path.genlib) catch {};
    defer cwd.deleteFile(path.genlibobject) catch {};

    const gen_t = *const fn (*Random, *MList(u8)) bool;
    var gen_lib = try DynLib.open(path.genlib);
    const gen = gen_lib.lookup(gen_t, "gen") orelse return log.err("load `gen` from {s} fail", .{path.genlib});
    log.info("load `gen` from {s}", .{path.genlib});

    var meta_random = Random.DefaultPrng.init(@intCast(time.nanoTimestamp()));
    var random = meta_random.random();

    for (0..1_000_000) |i| {
        var in = try MList(u8).initCapacity(alloc, 64);
        defer in.deinit();
        if (!gen(&random, &in)) return log.err("gen fail", .{});
        // log.info("generate {d} bytes", .{in.items.len});

        var main_child = try target.spawn(alloc, path.main_exe, in.items);
        var slow_cihld = try target.spawn(alloc, path.slow_exe, in.items);

        const out, const err = try target.wait(alloc, &main_child);
        defer alloc.free(out);
        alloc.free(err);
        const slow_out, const slow_err = try target.wait(alloc, &slow_cihld);
        defer alloc.free(slow_out);
        alloc.free(slow_err);

        // log.info("from main collect {d} bytes", .{out.len});
        // log.info("from slow collect {d} bytes", .{slow_out.len});

        if (mem.eql(u8, slow_out, out)) {
            log.info("fuzz {d} pass", .{i});
            continue;
        }

        log.info("fuzz {d} fail", .{i});
        var in_it = mem.tokenizeScalar(u8, in.items, '\n');
        while (in_it.next()) |line| log.info(term.italic ++ "  {s}" ++ term.reset, .{line});

        var width: usize = 0;
        var exp_out_it = mem.tokenizeScalar(u8, slow_out, '\n');
        while (exp_out_it.next()) |line| width = @max(width, 4 + line.len);
        exp_out_it.reset();

        var out_it = mem.tokenizeScalar(u8, out, '\n');
        while (exp_out_it.peek() != null or out_it.peek() != null) {
            const outex_line = exp_out_it.next() orelse "";
            const out_line = out_it.next() orelse "";

            var buf = try alloc.alloc(u8, width + out_line.len);
            defer alloc.free(buf);
            @memcpy(buf[0..outex_line.len], outex_line);
            @memset(buf[outex_line.len..width], ' ');
            @memcpy(buf[width..], out_line);
            log.info("  {s}", .{buf});
        }

        var err_it = mem.tokenizeScalar(u8, err, '\n');
        while (err_it.next()) |line| log.info("  (err) {s}", .{line});
        break;
    }
}

const term = @import("../term.zig");
const cxx = @import("../cxx.zig");
const zigc = @import("../zigc.zig");
const path = @import("../path.zig");
const target = @import("../target.zig");

const List = @import("std").ArrayListUnmanaged;
const MList = @import("std").ArrayList;
const Child = @import("std").process.Child;
const log = @import("std").log;
const fs = @import("std").fs;
const mem = @import("std").mem;
const time = @import("std").time;
const DynLib = @import("std").DynLib;
const Random = @import("std").Random;
