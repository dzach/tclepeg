#!/usr/bin/tclsh8.6

exec gcc -shared tclepeg.c /home/dz/develop/epeg/src/lib/.libs/libepeg.a -ljpeg -o tclepeg.so

load ./tclepeg.so

set fd [open test.jpeg]
fconfigure $fd -translation binary
puts [time {set img [read $fd]}]
close $fd

set fd [open out.jpeg w]
fconfigure $fd -translation binary
puts -nonewline $fd [epeg -max 64 -quality 50 $img]
close $fd
