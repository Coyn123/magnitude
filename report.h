#pragma once
#include <vector>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
namespace fs = std::filesystem;

std::vector<std::pair<fs::path, std::uintmax_t>> sort_memo(std::vector<std::pair<fs::path, std::uintmax_t>> entries);
std::vector<std::pair<fs::path, std::uintmax_t>> top_level_sizes(fs::path const& root, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes);
