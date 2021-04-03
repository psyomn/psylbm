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

# plans of improvement

I wrote the original version of this repo a long time ago, and over
the course of a few caffeine addled days. This repo is far from
perfect but I had a toy project in mind I wanted to write down for
fun. Here's an incomplete list of considerations that I've been
meaning to do but never got around to:

- get rid of the silly text protocol, and use more interesting
  encoding
- originally an academic/educational project, so the
  authentication/authorization aspect is superfluous. It will make
  more sense to scrap all that, and bind the service to `127.0.0.1`.
- daemonize service in userspace with easy to setup scripts(?)
- clean up a lot of code (I don't write C like this anymore :P)

# license

GPLv3.
