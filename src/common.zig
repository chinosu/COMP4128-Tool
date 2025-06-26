// pub fn cxx_compile(alloc: mem.Allocator, src: []const u8, exe: []const u8, opts: enum { fast, debug }) !bool {
//     var cxx = switch (opts) {
//         .fast => Child.init(
//             &.{
//                 "g++-15",
//                 src,
//                 "-o",
//                 exe,
//                 "-std=gnu++20",
//                 // "-g",
//                 "-O3",
//             },
//             alloc,
//         ),
//         .debug => Child.init(
//             &.{
//                 "g++-15",
//                 src,
//                 "-o",
//                 exe,
//                 "-std=gnu++20",
//                 // "-g",
//                 "-O2",
//                 "-W" ++ "float-equal",
//                 "-W" ++ "logical-op",
//                 "-f" ++ "diagnostics-color=always",
//                 "-f" ++ "no-sanitize-recover",
//                 "-f" ++ "stack-protector",
//                 "-D" ++ "_GLIBCXX_DEBUG",
//                 "-D" ++ "_GLIBCXX_DEBUG_PEDANTIC",
//                 "-D" ++ "_FORTIFY_SOURCE=2",
//                 "-D" ++ "__DEBUG__",
//             },
//             alloc,
//         ),
//     };
//     cxx.stdin_behavior = .Close;
//     cxx.stdout_behavior = .Close;
//     cxx.stderr_behavior = .Pipe;

//     try cxx.spawn();
//     const err_buf = try alloc.alloc(u8, 8192);
//     defer alloc.free(err_buf);
//     const err = err_buf[0..try cxx.stderr.?.readAll(err_buf)];

//     if (err.len != 0) {
//         var lines = mem.tokenizeScalar(u8, err, '\n');
//         while (lines.next()) |line| log.warn("(cxx) {s}", .{line});
//     }

//     switch ((try cxx.wait()).Exited) {
//         0 => {
//             log.info("compile success {s}", .{src});
//             return true;
//         },
//         else => |c| {
//             log.err("compile exit {d} for {s}", .{ c, src });
//             return false;
//         },
//     }
// }

// const Child = @import("std").process.Child;
// const log = @import("std").log;
// const mem = @import("std").mem;
