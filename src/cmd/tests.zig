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
        var proc = Child.init(&.{path.exe}, alloc);
        proc.stdin_behavior = .Pipe;
        proc.stdout_behavior = .Pipe;
        proc.stderr_behavior = .Pipe;
        try proc.spawn();
        try proc.stdin.?.writeAll(in.items[i].items);

        var poll = io.poll(alloc, enum { out, err }, .{ .out = proc.stdout.?, .err = proc.stderr.? });
        defer poll.deinit();
        while (try poll.poll()) {
            if (poll.fifo(.out).count > 8192) {
                log.warn("test {d} stopped ({d} bytes from stdout)", .{ i + 1, poll.fifo(.out).count });
                _ = try proc.kill();
            }
            if (poll.fifo(.err).count > 8192) {
                log.warn("test {d} stopped ({d} bytes from stderr)", .{ i + 1, poll.fifo(.err).count });
                _ = try proc.kill();
            }
        }
        const out = poll.fifo(.out).buf[0..poll.fifo(.out).count];
        const err = poll.fifo(.err).buf[0..poll.fifo(.err).count];
        _ = try proc.wait();

        if (mem.eql(u8, exp_out.items[i].items, out)) {
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

const path = @import("../path.zig");
const term = @import("../term.zig");
const cxx_compile = @import("../common.zig").cxx_compile;

const List = @import("std").ArrayListUnmanaged;
const assert = @import("std").debug.assert;
const Child = @import("std").process.Child;
const log = @import("std").log;
const io = @import("std").io;
const fs = @import("std").fs;
const mem = @import("std").mem;
