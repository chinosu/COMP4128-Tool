pub fn tests(alloc: mem.Allocator) !void {
    const cwd = fs.cwd();

    var cxx_child = try cxx.spawn(alloc, path.main_src, path.main_exe, .debug);
    try cxx.wait(alloc, &cxx_child, path.main_src);
    defer cwd.deleteFile(path.main_exe) catch {};

    var runs = try find_tests(alloc, path.tests);
    defer runs.deinit(alloc);
    defer for (0..runs.items.len) |i| {
        runs.items[i].in.deinit(alloc);
        runs.items[i].out.deinit(alloc);
    };

    for (runs.items, 1..) |run, i| {
        var child = try target.spawn(alloc, path.main_exe, run.in.items);
        const out, const err = try target.wait(alloc, &child);
        defer alloc.free(out);
        defer alloc.free(err);

        if (mem.eql(u8, run.out.items, out)) {
            log.info("test {d} pass", .{i});
            continue;
        }

        log.info("test {d} fail", .{i});
        var in_it = mem.tokenizeScalar(u8, run.in.items, '\n');
        while (in_it.next()) |line| log.info(term.italic ++ "  {s}" ++ term.reset, .{line});

        var width: usize = 0;
        var exp_out_it = mem.tokenizeScalar(u8, run.out.items, '\n');
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
    }
}

const Run = struct { in: List(u8), out: List(u8) };
fn find_tests(alloc: mem.Allocator, test_path: []const u8) !List(Run) {
    const file = try fs.cwd().openFile(test_path, .{});
    defer file.close();

    const bytes = try file.readToEndAlloc(alloc, 4096);
    defer alloc.free(bytes);
    log.info("read {d} bytes from test file", .{bytes.len});

    var runs = try List(Run).initCapacity(alloc, 32);

    var lines = mem.tokenizeScalar(u8, bytes, '\n');
    while (lines.next()) |_line| {
        const line = mem.trim(u8, _line, " \t\r\n");
        if (line.len == 0 or line[0] == '#') continue;

        if (line[0] == '|') {
            if (runs.items.len == 0 or runs.items[runs.items.len - 1].out.items.len != 0) {
                try runs.append(alloc, .{
                    .in = try List(u8).initCapacity(alloc, 64),
                    .out = try List(u8).initCapacity(alloc, 64),
                });
            }
            try runs.items[runs.items.len - 1].in.appendSlice(alloc, mem.trimLeft(u8, line[1..], " "));
            try runs.items[runs.items.len - 1].in.append(alloc, '\n');
        } else {
            try runs.items[runs.items.len - 1].out.appendSlice(alloc, line);
            try runs.items[runs.items.len - 1].out.append(alloc, '\n');
        }
    }

    if (runs.items.len != 1) {
        log.info("find {d} tests", .{runs.items.len});
    } else {
        log.info("find 1 test", .{});
    }

    return runs;
}

const path = @import("../path.zig");
const term = @import("../term.zig");
const cxx = @import("../cxx.zig");
const target = @import("../target.zig");

const List = @import("std").ArrayListUnmanaged;
const assert = @import("std").debug.assert;
const Child = @import("std").process.Child;
const log = @import("std").log;
const io = @import("std").io;
const fs = @import("std").fs;
const mem = @import("std").mem;
