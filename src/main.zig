const std = @import("std");
const builtin = @import("builtin");

const log = std.log;
const mem = std.mem;
const process = std.process;
const heap = std.heap;
const Random = std.Random;
const time = std.time;
const fs = std.fs;
const fmt = std.fmt;
const DynLib = std.DynLib;

const List = std.ArrayListUnmanaged;
const MList = std.ArrayList;
const assert = std.debug.assert;

const src_path = "./main.cc";
const exe_path = "./main";
const test_path = "./tests.txt";
const gen_path = "./gen.zig";
const genlib_path = "libgen.dylib";
const slow_src_path = "./slow.cc";
const slow_exe_path = "./slow";

pub fn main() !void {
    var debug_alloc = if (builtin.mode == .Debug) heap.DebugAllocator(.{ .safety = true }).init;
    defer if (builtin.mode == .Debug) assert(debug_alloc.deinit() == .ok);
    const alloc = if (builtin.mode == .Debug) debug_alloc.allocator() else heap.smp_allocator;

    var args = process.argsWithAllocator(alloc) catch unreachable;
    defer args.deinit();
    assert(args.skip());
    const cmd = args.next() orelse return log.err("enter a command", .{});

    if (mem.eql(u8, cmd, "ini")) {
        const name = args.next() orelse return log.err("you must specify a name", .{});
        try cmd_ini(name);
    } else if (mem.eql(u8, cmd, "t") or mem.eql(u8, cmd, "test")) {
        try cmd_test(alloc);
    } else if (mem.eql(u8, cmd, "f") or mem.eql(u8, cmd, "fuzz")) {
        try cmd_fuzz(alloc);
    } else {
        return log.err("unrecognized command '{s}'", .{cmd});
    }
}

fn cmd_ini(name: []const u8) !void {
    const dir = fs.cwd();

    dir.makeDir(name) catch |e| switch (e) {
        error.PathAlreadyExists => {},
        else => return e,
    };

    var subdir = try dir.openDir(name, .{});
    defer subdir.close();

    _ = subdir.statFile(src_path) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(src_path, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/main.cc"));
        },
        else => return e,
    };
    _ = subdir.statFile(test_path) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(test_path, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/tests.txt"));
        },
        else => return e,
    };
    _ = subdir.statFile(gen_path) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(gen_path, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/gen.zig"));
        },
        else => return e,
    };
    _ = subdir.statFile(slow_src_path) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(slow_src_path, .{});
            defer f.close();
            try f.writeAll(@embedFile("starterfiles/main.cc"));
        },
        else => return e,
    };
}

fn cmd_test(alloc: mem.Allocator) !void {
    if (!try cxx_compile(alloc, src_path, exe_path, .slow)) return;

    const cwd = fs.cwd();
    const test_file = try cwd.openFile(test_path, .{});
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
        var proc = process.Child.init(&.{exe_path}, alloc);
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

fn cmd_fuzz(alloc: mem.Allocator) !void {
    if (!try cxx_compile(alloc, src_path, exe_path, .fast)) return;
    if (!try cxx_compile(alloc, slow_src_path, slow_exe_path, .fast)) return;

    var zig = process.Child.init(&.{ "zig", "build-lib", gen_path, "-dynamic" }, alloc);
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
        0 => log.info("compile success {s}", .{gen_path}),
        else => |c| {
            log.err("compile exit {d} for {s}", .{ c, gen_path });
            return;
        },
    }

    const gen_t = *const fn (*Random, *MList(u8)) bool;
    var gen_lib = try DynLib.open(genlib_path);
    const gen = gen_lib.lookup(gen_t, "gen") orelse return log.err("find `gen` fn from dylib fail", .{});
    log.info("found `gen` from dylib", .{});

    var meta_random = Random.DefaultPrng.init(@intCast(time.nanoTimestamp()));
    var random = meta_random.random();
    var in = try MList(u8).initCapacity(alloc, 64);
    defer in.deinit();
    if (!gen(&random, &in)) return log.err("gen fail", .{});
    log.info("generate {d} bytes", .{in.items.len});

    var proc = process.Child.init(&.{exe_path}, alloc);
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

    var slow_proc = process.Child.init(&.{slow_exe_path}, alloc);
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

const term = struct {
    fn ansiCount(args: anytype, comptime terminator: []const u8) u64 {
        comptime {
            const fields = @typeInfo(@TypeOf(args)).@"struct".fields;
            var len = 2 + fields.len - 1;
            for (fields) |field| len += fmt.count("{any}", .{@field(args, field.name)});
            len += terminator.len;
            return len;
        }
    }

    fn ansi(args: anytype, terminator: []const u8) *const [ansiCount(args, terminator)]u8 {
        comptime {
            const fields = @typeInfo(@TypeOf(args)).@"struct".fields;

            var buf: [ansiCount(args, terminator)]u8 = undefined;
            buf[0] = '\x1b';
            buf[1] = '[';
            var b = 2;

            for (fields, 0..) |field, i| {
                b += (fmt.bufPrint(
                    buf[b..],
                    "{any}",
                    .{@field(args, field.name)},
                ) catch unreachable).len;

                if (i != fields.len - 1) {
                    buf[b] = ';';
                    b += 1;
                }
            }

            @memcpy(buf[b..], terminator);

            const final = buf;
            return &final;
        }
    }

    const reset = ansi(.{0}, "m");
    const bold = ansi(.{1}, "m");
    const dim = ansi(.{2}, "m");
    const italic = ansi(.{3}, "m");
    const underline = ansi(.{4}, "m");
    const blink = ansi(.{5}, "m");
    const inverse = ansi(.{7}, "m");
    const hidden = ansi(.{8}, "m");
    const strikethrough = ansi(.{9}, "m");

    const fg = struct {
        const black = ansi(.{30}, "m");
        const red = ansi(.{31}, "m");
        const green = ansi(.{32}, "m");
        const yellow = ansi(.{33}, "m");
        const blue = ansi(.{34}, "m");
        const magenta = ansi(.{35}, "m");
        const cyan = ansi(.{36}, "m");
        const white = ansi(.{37}, "m");

        const black_bright = ansi(.{90}, "m");
        const red_bright = ansi(.{91}, "m");
        const green_bright = ansi(.{92}, "m");
        const yellow_bright = ansi(.{93}, "m");
        const blue_bright = ansi(.{94}, "m");
        const magenta_bright = ansi(.{95}, "m");
        const cyan_bright = ansi(.{96}, "m");
        const white_bright = ansi(.{97}, "m");
    };

    const bg = struct {
        const black = ansi(.{40}, "m");
        const red = ansi(.{41}, "m");
        const green = ansi(.{42}, "m");
        const yellow = ansi(.{43}, "m");
        const blue = ansi(.{44}, "m");
        const magenta = ansi(.{45}, "m");
        const cyan = ansi(.{46}, "m");
        const white = ansi(.{47}, "m");

        const black_bright = ansi(.{100}, "m");
        const red_bright = ansi(.{101}, "m");
        const green_bright = ansi(.{102}, "m");
        const yellow_bright = ansi(.{103}, "m");
        const blue_bright = ansi(.{104}, "m");
        const magenta_bright = ansi(.{105}, "m");
        const cyan_bright = ansi(.{106}, "m");
        const white_bright = ansi(.{107}, "m");
    };
};
