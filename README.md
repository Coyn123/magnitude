# magnitude

A small C++ tool for finding what's eating your disk space. Scans a directory (multithreaded), traces the single heaviest folder chain down to the largest file inside it, then lists every top-level folder's size for a full breakdown.

## Build

Requires a C++17 compiler with `<filesystem>` and `<thread>` support (tested with g++ via MSYS2 UCRT64 on Windows).

```bash
g++ -std=c++17 -Wall -Wextra -static main.cpp filesize.cpp report.cpp threadpool.cpp -o magnitude.exe
```

`-static` bundles the C++ runtime into the executable so it runs on machines without MSYS2/MinGW installed.

## Usage

```bash
magnitude.exe [path]
```

If `path` is omitted, it scans the current directory.
