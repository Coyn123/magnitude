#include "report.h"
#include <vector>
#include <algorithm>
#include <string>

std::vector<std::pair<fs::path, std::uintmax_t>> sort_memo(std::vector<std::pair<fs::path, std::uintmax_t>> entries) {

    // CPP lambda, also didn't write this: a / b are key/value essentially
    // sorts the array largest to smallest
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return entries;

}

std::vector<std::pair<fs::path, std::uintmax_t>> top_level_sizes(fs::path const& root, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes) {

    std::vector<std::pair<fs::path, std::uintmax_t>> printTree;

    try {
        auto it = fs::directory_iterator(root);
        std::error_code ec;

        while ( it != fs::directory_iterator{} ) {
            const auto& pointer = *it;
            if (pointer.is_directory()) {
                printTree.push_back({pointer.path(), memo_sizes[pointer.path()]});
            }
            it.increment(ec);
            if(!ec) continue; else return printTree;
        }

        return printTree;

    }
    catch (const fs::filesystem_error& e) {
        return printTree;
    }
}
