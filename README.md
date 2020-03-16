# psylightbookmark ![build](https://github.com/psyomn/psylbm/workflows/Build%20and%20Test/badge.svg)

This is a very small application that aims to be a light server, with a tiny
protocol, that aims to store bookmarks of various media, be it books, manga,
comics, whatever.

Don't treat the software as secure. This was coded for fun, and maybe aspects
of security might have been neglected.

The protocol will be provided, and whoever wants to use this tiny server can
implement something in their own application. If you're interested, see the
file PROTOCOL.

# compiling

You can compile in debug mode with:

    mkdir -p build/debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug

And similarly for release:

    mkdir -p build/release
    cmake .. -DCMAKE_BUILD_TYPE=Release

# authors

Simon (psyomn) Symeonidis (2014)

# license

GPLv3.
