pub const reset = ansi(.{0}, "m");
pub const bold = ansi(.{1}, "m");
pub const dim = ansi(.{2}, "m");
pub const italic = ansi(.{3}, "m");
pub const underline = ansi(.{4}, "m");
pub const blink = ansi(.{5}, "m");
pub const inverse = ansi(.{7}, "m");
pub const hidden = ansi(.{8}, "m");
pub const strikethrough = ansi(.{9}, "m");

pub const fg = struct {
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

pub const bg = struct {
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

fn ansiCount(args: anytype, comptime terminator: []const u8) u64 {
    comptime {
        const fields = @typeInfo(@TypeOf(args)).@"struct".fields;
        var len = 2 + fields.len - 1;
        for (fields) |field| len += fmt.count("{any}", .{@field(args, field.name)});
        len += terminator.len;
        return len;
    }
}

const fmt = std.fmt;
const std = @import("std");
