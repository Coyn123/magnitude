#pragma once
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <string>
namespace fs = std::filesystem;

constexpr std::uintmax_t KB = 1024;
constexpr std::uintmax_t MB = KB * 1024;
constexpr std::uintmax_t GB = MB * 1024;

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

std::uintmax_t total_size_of(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& sizes);

BiggestEntry biggest_child_of(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& sizes);
