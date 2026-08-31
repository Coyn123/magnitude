#pragma once
#include <vector>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "threadpool.h"
namespace fs = std::filesystem;

constexpr std::uintmax_t KB = 1024;
constexpr std::uintmax_t MB = KB * 1024;
constexpr std::uintmax_t GB = MB * 1024;

extern std::mutex sizes_mutex;

const std::unordered_set<std::string> excludes = {
    "System Volume Information",
    "$Recycle.Bin",
    "WindowsApps",
    "Recovery",
    "Config.Msi",
    "$WinREAgent",
    "pagefile.sys",
    "hiberfil.sys",
    "swapfile.sys",
    "Temporary Internet Files",
    "INetCache",
    "PerfLogs",
    "Documents and Settings",
    "Windows Defender Advanced Threat Protection",
    "Microsoft",
    "System32",
    "SysWOW64",
    "DriverStore"
};

struct BiggestEntry {
    fs::path path_to_biggest;
    std::uintmax_t size_of_biggest = 0;
};

BiggestEntry biggest_child_of(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& sizes);


void rollup_sizes(std::unordered_map<fs::path, std::uintmax_t>& memo_sizes);
void scan_task(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes, ThreadPool& pool);
std::vector<std::pair<fs::path, std::uintmax_t>> sort_memo(std::vector<std::pair<fs::path, std::uintmax_t>> entries);
std::vector<std::pair<fs::path, std::uintmax_t>> top_level_sizes(fs::path const& root, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes);
