#include "report.h"
#include "filesize.h"
#include <vector>
#include <algorithm>
#include <string>
#include <mutex>

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


void scan_task(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes, ThreadPool& pool) {
    std::error_code ec;
    try {
        std::uintmax_t memo_sizes_local = 0;
        auto it = fs::directory_iterator(path);
        while (it != fs::directory_iterator{}) {

            const auto& file = *it;

            if( excludes.count(file.path().filename().string()) ) {
                it.increment(ec);
                if(!ec) continue; else {
                    return;
                }
            }


            if(file.is_regular_file()) memo_sizes_local += file.file_size();
            if(file.is_directory()) {
                pool.submit([child = file.path(), &memo_sizes, &pool]() { scan_task(child, memo_sizes, pool); });
            }

            it.increment(ec);
            if(!ec) continue; else return;
        }

        {
            std::unique_lock<std::mutex> lock(sizes_mutex);
            memo_sizes[path] = memo_sizes_local;
        }

    }
    catch(const fs::filesystem_error& e) {
        return;
    }
}

void rollup_sizes(std::unordered_map<fs::path, std::uintmax_t>& memo_sizes) {

    std::vector<fs::path> local_paths;
    for(auto& [key, value] : memo_sizes) {
        local_paths.push_back(key);
    }
    std::sort(local_paths.begin(), local_paths.end(), [](const auto& a, const auto& b) {
        return std::distance(a.begin(), a.end()) > std::distance(b.begin(), b.end());
    });

    for(auto& path : local_paths) {
        fs::path par = path.parent_path();

        {
            std::unique_lock<std::mutex> lock(sizes_mutex);
            if(memo_sizes.count(par)) {
                memo_sizes[par] += memo_sizes[path];
            }
        }
    }
}
