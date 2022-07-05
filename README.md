# Avalanche
## About
Avalanche is a
[byte-compiled](https://en.wikipedia.org/wiki/Java_bytecode) [general
purpose](https://en.wikipedia.org/wiki/High-level_programming_language)
[high-level](https://en.wikipedia.org/wiki/General-purpose_language)
[imperative](https://en.wikipedia.org/wiki/Imperative_programming)
programming language designed and written by Roland Marchand.

## Compiling
Avalanche uses CMake and is written for Linux with glibc.

TODO: find the minimum compatible build.

To build, run those commands in order.
```
cd /path/to/moolang
mkdir build
cmake -S . -B build
cd build
make -j$(nproc)
```
## License
```
Copyright (c) 2022, Roland Marchand <roland.marchand@protonmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
