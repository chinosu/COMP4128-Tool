pub fn spawn(alloc: mem.Allocator, src: []const u8, exe: []const u8, opts: enum { fast, debug }) !Child {
    var child = switch (opts) {
        .fast => Child.init(&.{
            "g++-15",
            src,
            "-o",
            exe,
            "-std=gnu++20",
            "-O3",
        }, alloc),
        .debug => Child.init(&.{
            "g++-15",
            src,
            "-o",
            exe,
            "-std=gnu++20",
            "-O2",
            "-W" ++ "float-equal",
            "-W" ++ "logical-op",
            // "-W" ++ "shadow",
            "-f" ++ "diagnostics-color=always",
            "-f" ++ "no-sanitize-recover",
            "-f" ++ "stack-protector",
            "-D" ++ "_GLIBCXX_DEBUG",
            "-D" ++ "_GLIBCXX_DEBUG_PEDANTIC",
            "-D" ++ "_FORTIFY_SOURCE=2",
            "-D" ++ "__DEBUG__",
        }, alloc),
    };
    child.stdin_behavior = .Close;
    child.stdout_behavior = .Close;
    child.stderr_behavior = .Pipe;
    try child.spawn();
    return child;
}

pub fn wait(alloc: mem.Allocator, child: *Child, src: []const u8) !void {
    const buf = try alloc.alloc(u8, 8192);
    defer alloc.free(buf);
    const err = buf[0..try child.stderr.?.readAll(buf)];

    if (err.len != 0) {
        var lines = mem.splitScalar(u8, err, '\n');
        while (lines.next()) |line| log.warn("(cxx) {s}", .{line});
    }

    switch ((try child.wait()).Exited) {
        0 => log.info("compile success {s}", .{src}),
        else => |c| {
            log.err("compile exit {d} for {s}", .{ c, src });
            return error.CXXExitNonZero;
        },
    }
}

const Child = @import("std").process.Child;
const log = @import("std").log;
const mem = @import("std").mem;
