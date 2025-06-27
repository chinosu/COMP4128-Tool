pub fn spawn(alloc: mem.Allocator, exe: []const u8, in: []const u8) !Child {
    var child = Child.init(&.{exe}, alloc);
    child.stdin_behavior = .Pipe;
    child.stdout_behavior = .Pipe;
    child.stderr_behavior = .Pipe;
    try child.spawn();
    try child.stdin.?.writeAll(in);

    return child;
}

pub fn wait(alloc: mem.Allocator, child: *Child) !struct { []u8, []u8 } {
    var poll = io.poll(
        alloc,
        enum { out, err },
        .{ .out = child.stdout.?, .err = child.stderr.? },
    );
    defer poll.deinit();

    while (try poll.poll()) {
        if (poll.fifo(.out).count > 8192) {
            log.warn("reach stdout byte limit ({d})", .{poll.fifo(.out).count});
            _ = try child.kill();
        }
        if (poll.fifo(.err).count > 8192) {
            log.warn("reach stderr byte limit ({d})", .{poll.fifo(.err).count});
            _ = try child.kill();
        }
    }

    const out = try alloc.dupe(u8, poll.fifo(.out).buf[0..poll.fifo(.out).count]);
    const err = try alloc.dupe(u8, poll.fifo(.err).buf[0..poll.fifo(.err).count]);
    _ = try child.wait();
    return .{ out, err };
}

const Child = @import("std").process.Child;
const log = @import("std").log;
const io = @import("std").io;
const mem = @import("std").mem;
