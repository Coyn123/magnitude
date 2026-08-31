# magnitude

A small C++ tool for finding what's eating your disk space. Scans a directory (multithreaded), traces the single heaviest folder chain down to the largest file inside it, then lists every top-level folder's size for a full breakdown.

https://coyner-ne.blogspot.com/2026/08/magnitude-c-disk-space-tool-built-to.html

## Build

Requires a C++17 compiler with `<filesystem>` and `<thread>` support.

Alternatively, download the v1.0.0 GitHub build on the side of the page

### Windows (tested with g++ via MSYS2 UCRT64)

```bash
g++ -std=c++17 -Wall -Wextra -static main.cpp report.cpp threadpool.cpp -o magnitude.exe
```

`-static` bundles the C++ runtime into the executable so it runs on machines without MSYS2/MinGW installed.

### POSIX (Linux/macOS)

```bash
g++ -std=c++17 -Wall -Wextra main.cpp report.cpp threadpool.cpp -o magnitude
```

Drop `-static`: macOS's toolchain doesn't support fully static-linking against the system libc, and it's unnecessary on Linux/macOS since the C++ runtime is normally already present. Output can be named `magnitude` or `magnitude.exe` — POSIX doesn't treat the extension specially.

## Usage

```bash
magnitude.exe [path]
```

If `path` is omitted, it scans the current directory.
