const Self = @This();

alloc: mem.Allocator,
compiler: Child,
src: []const u8,
exe: []const u8,

pub fn spawn(alloc: mem.Allocator, src: []const u8, exe: []const u8, opts: enum { fast, debug }) !Self {
    var compiler = switch (opts) {
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
            "-f" ++ "diagnostics-color=always",
            "-f" ++ "no-sanitize-recover",
            "-f" ++ "stack-protector",
            "-D" ++ "_GLIBCXX_DEBUG",
            "-D" ++ "_GLIBCXX_DEBUG_PEDANTIC",
            "-D" ++ "_FORTIFY_SOURCE=2",
            "-D" ++ "__DEBUG__",
        }, alloc),
    };
    compiler.stdin_behavior = .Close;
    compiler.stdout_behavior = .Close;
    compiler.stderr_behavior = .Pipe;
    try compiler.spawn();
    return .{ .alloc = alloc, .compiler = compiler, .src = src, .exe = exe };
}

pub fn wait(self: *Self) !void {
    const buf = try self.alloc.alloc(u8, 8192);
    defer self.alloc.free(buf);
    const err = buf[0..try self.compiler.stderr.?.readAll(buf)];

    if (err.len != 0) {
        var lines = mem.splitScalar(u8, err, '\n');
        while (lines.next()) |line| log.warn("(cxx) {s}", .{line});
    }

    switch ((try self.compiler.wait()).Exited) {
        0 => log.info("compile success {s}", .{self.src}),
        else => |c| {
            log.err("compile exit {d} for {s}", .{ c, self.src });
            return error.CXXExitNonZero;
        },
    }
}

const Child = @import("std").process.Child;
const log = @import("std").log;
const fs = @import("std").fs;
const mem = @import("std").mem;
