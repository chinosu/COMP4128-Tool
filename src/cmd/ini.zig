pub fn ini(name: []const u8) !void {
    const dir = fs.cwd();

    dir.makeDir(name) catch |e| switch (e) {
        error.PathAlreadyExists => {},
        else => return e,
    };
    var subdir = try dir.openDir(name, .{});
    defer subdir.close();

    _ = subdir.statFile(path.main_src) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.main_src, .{});
            defer f.close();
            try f.writeAll(@embedFile("../starterfiles/main.cc"));
        },
        else => return e,
    };
    // _ = subdir.statFile(path.lib_src) catch |e| switch (e) {
    //     error.FileNotFound => {
    //         const f = try subdir.createFile(path.lib_src, .{});
    //         defer f.close();
    //         try f.writeAll(@embedFile("../starterfiles/lib.cc"));
    //     },
    //     else => return e,
    // };
    _ = subdir.statFile(path.tests) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.tests, .{});
            defer f.close();
            try f.writeAll(@embedFile("../starterfiles/tests.txt"));
        },
        else => return e,
    };
    _ = subdir.statFile(path.gen) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.gen, .{});
            defer f.close();
            try f.writeAll(@embedFile("../starterfiles/gen.zig"));
        },
        else => return e,
    };
    _ = subdir.statFile(path.slow_src) catch |e| switch (e) {
        error.FileNotFound => {
            const f = try subdir.createFile(path.slow_src, .{});
            defer f.close();
            try f.writeAll(@embedFile("../starterfiles/main.cc"));
        },
        else => return e,
    };
}

pub fn ini_code(name: []const u8, alloc: Allocator) !void {
    try ini(name);
    _ = try run(.{
        .allocator = alloc,
        .argv = &.{ "code", name, "--reuse-window" },
        .expand_arg0 = .expand,
    });
}

const path = @import("../path.zig");
const fs = @import("std").fs;
const Allocator = @import("std").mem.Allocator;
const run = @import("std").process.Child.run;
