pub fn ini(name: []const u8) !void {
    const dir = fs.cwd();

    dir.makeDir(name) catch |e| switch (e) {
        error.PathAlreadyExists => {},
        else => return e,
    };

    var subdir = try dir.openDir(name, .{});
    defer subdir.close();

    _ = subdir.statFile(path.src) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.src, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/main.cc"));
        },
        else => return e,
    };
    _ = subdir.statFile(path.tests) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.tests, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/tests.txt"));
        },
        else => return e,
    };
    _ = subdir.statFile(path.gen) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.gen, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/gen.zig"));
        },
        else => return e,
    };
    _ = subdir.statFile(path.slow_src) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.slow_src, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/main.cc"));
        },
        else => return e,
    };
}

pub fn tests(alloc: mem.Allocator) !void {
    if (!try cxx_compile(alloc, path.src, path.exe, .slow)) return;

    const cwd = fs.cwd();
    const test_file = try cwd.openFile(path.tests, .{});
    defer test_file.close();
    const test_bytes = try test_file.readToEndAlloc(alloc, 2048);
    defer alloc.free(test_bytes);
    log.info("read {d} bytes from test file", .{test_bytes.len});

    var in = try List(List(u8)).initCapacity(alloc, 32);
    defer in.deinit(alloc);
    defer for (0..in.items.len) |i| in.items[i].deinit(alloc);

    var exp_out = try List(List(u8)).initCapacity(alloc, 32);
    defer exp_out.deinit(alloc);
    defer for (0..exp_out.items.len) |i| exp_out.items[i].deinit(alloc);

    var lines = mem.tokenizeScalar(u8, test_bytes, '\n');
    while (lines.next()) |_line| {
        const line = mem.trim(u8, _line, " \t\r\n");
        if (line.len == 0 or mem.startsWith(u8, line, "#")) continue;

        if (mem.startsWith(u8, line, "| ")) {
            if (exp_out.items.len == 0 or exp_out.items[exp_out.items.len - 1].items.len != 0) {
                try in.append(alloc, try List(u8).initCapacity(alloc, 64));
                try exp_out.append(alloc, try List(u8).initCapacity(alloc, 64));
            }
            try in.items[in.items.len - 1].appendSlice(alloc, line[2..]);
            try in.items[in.items.len - 1].append(alloc, '\n');
        } else {
            try exp_out.items[exp_out.items.len - 1].appendSlice(alloc, line);
            try exp_out.items[exp_out.items.len - 1].append(alloc, '\n');
        }
    }

    assert(in.items.len == exp_out.items.len);
    if (in.items.len != 1) {
        log.info("found {d} tests", .{in.items.len});
    } else {
        log.info("found 1 test", .{});
    }

    for (0..in.items.len) |i| {
        var proc = process.Child.init(&.{path.exe}, alloc);
        proc.stdin_behavior = .Pipe;
        proc.stdout_behavior = .Pipe;
        proc.stderr_behavior = .Pipe;
        var out = try List(u8).initCapacity(alloc, 128);
        defer out.deinit(alloc);
        var err = try List(u8).initCapacity(alloc, 128);
        defer err.deinit(alloc);
        try proc.spawn();
        try proc.stdin.?.writeAll(in.items[i].items);
        try proc.collectOutput(alloc, &out, &err, 8192);
        _ = try proc.wait();

        if (mem.eql(u8, exp_out.items[i].items, out.items)) {
            log.info("test {d} pass", .{i + 1});
            continue;
        }

        log.info("test {d} fail", .{i + 1});
        var in_it = mem.tokenizeScalar(u8, in.items[i].items, '\n');
        while (in_it.next()) |line| log.info(term.italic ++ "  {s}" ++ term.reset, .{line});

        var width: usize = 0;
        var exp_out_it = mem.tokenizeScalar(u8, exp_out.items[i].items, '\n');
        while (exp_out_it.next()) |line| width = @max(width, 4 + line.len);
        exp_out_it.reset();

        var out_it = mem.tokenizeScalar(u8, out.items, '\n');
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

        var err_it = mem.tokenizeScalar(u8, err.items, '\n');
        while (err_it.next()) |line| log.info("  (err) {s}", .{line});
    }
}

pub fn fuzz(alloc: mem.Allocator) !void {
    if (!try cxx_compile(alloc, path.src, path.exe, .fast)) return;
    if (!try cxx_compile(alloc, path.slow_src, path.slow_exe, .fast)) return;

    var zig = process.Child.init(&.{ "zig", "build-lib", path.gen, "-dynamic" }, alloc);
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
        else => |c| {
            log.err("compile exit {d} for {s}", .{ c, path.gen });
            return;
        },
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

    var proc = process.Child.init(&.{path.exe}, alloc);
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

    var slow_proc = process.Child.init(&.{path.slow_exe}, alloc);
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

fn cxx_compile(alloc: mem.Allocator, src: []const u8, exe: []const u8, opts: enum { fast, slow }) !bool {
    var cxx = switch (opts) {
        .fast => process.Child.init(
            &.{
                "g++-15",
                src,
                "-o",
                exe,
                "-std=gnu++20",
                // "-g",
                "-O3",
            },
            alloc,
        ),
        .slow => process.Child.init(
            &.{
                "g++-15",
                src,
                "-o",
                exe,
                "-std=gnu++20",
                // "-g",
                "-O2",
                "-W" ++ "float-equal",
                "-W" ++ "logical-op",
                "-f" ++ "diagnostics-color=always",
                "-f" ++ "no-sanitize-recover",
                "-f" ++ "stack-protector",
                "-D" ++ "_GLIBCXX_DEBUG",
                "-D" ++ "_GLIBCXX_DEBUG_PEDANTIC",
                "-D" ++ "_FORTIFY_SOURCE=2",
                "-D" ++ "__DEBUG__",
            },
            alloc,
        ),
    };
    cxx.stdin_behavior = .Close;
    cxx.stdout_behavior = .Close;
    cxx.stderr_behavior = .Pipe;

    try cxx.spawn();
    const full_err = try alloc.alloc(u8, 8192);
    defer alloc.free(full_err);
    const err = full_err[0..try cxx.stderr.?.readAll(full_err)];

    if (err.len != 0) {
        var lines = mem.tokenizeScalar(u8, err, '\n');
        while (lines.next()) |line| log.warn("(cxx) {s}", .{line});
    }

    switch ((try cxx.wait()).Exited) {
        0 => {
            log.info("compile success {s}", .{src});
            return true;
        },
        else => |c| {
            log.err("compile exit {d} for {s}", .{ c, src });
            return false;
        },
    }
}

const path = @import("path.zig");
const term = @import("term.zig");

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
