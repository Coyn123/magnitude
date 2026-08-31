#include "filesize.h"
#include "report.h"
#include "threadpool.h"
#include <iostream>
#include <mutex>
#include <string>
#include <future>
#include <vector>
#include <algorithm>
#include <thread>


std::string display_correctly(std::uintmax_t value) {

    std::string tag;

    if (value / GB > 0) {
        value = static_cast<double>(value) / GB;
        tag = "GB";
    } else if (value / MB > 0) {
        value = static_cast<double>(value) / MB;
        tag = "MB";
    } else if (value / KB > 0) {
        value = static_cast<double>(value) / KB;
        tag = "KB";
    } else {
        value = value;
        tag = "bytes";
    }

    std::string strValue = " --> " + std::to_string(static_cast<unsigned long long>(value));
    return strValue + "" + tag;
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
            //I need help with the lambda sent to submit for sure.
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


int main(int argc, char* argv[]) {

    fs::path path;
    if(argc < 2) { path = fs::current_path(); } else { path = argv[1]; }
    std::cout << "Magnitude assessment..." << std::endl;
    BiggestEntry returnEntry;
    std::error_code ec;


    try {

        std::unordered_map<fs::path, std::uintmax_t> memo_sizes;
        std::vector<std::future<std::uintmax_t>> futures;

        size_t thread_size = std::thread::hardware_concurrency();

        ThreadPool thread_pool(thread_size);

        thread_pool.submit([path, &memo_sizes, &thread_pool]() { scan_task(path, memo_sizes, thread_pool); });

        thread_pool.wait_idle();
        rollup_sizes(memo_sizes);

        //total_size_of(path, memo_sizes);

        BiggestEntry winner;
        winner = biggest_child_of(path, memo_sizes);
        returnEntry.path_to_biggest = winner.path_to_biggest;
        returnEntry.size_of_biggest = winner.size_of_biggest;

        std::string disp = display_correctly(returnEntry.size_of_biggest);

        std::cout << returnEntry.path_to_biggest.string() << disp << std::endl;


        //Display the tree for the largest:
        while( fs::is_directory(winner.path_to_biggest) ) {
            winner = biggest_child_of(winner.path_to_biggest, memo_sizes);
            returnEntry.path_to_biggest = winner.path_to_biggest;
            returnEntry.size_of_biggest = winner.size_of_biggest;
            std::string disp = display_correctly(returnEntry.size_of_biggest);

            std::cout << returnEntry.path_to_biggest.string() << disp << std::endl;
        }


        std::vector<std::pair<fs::path, std::uintmax_t>> entries = top_level_sizes(path, memo_sizes);

        std::vector<std::pair<fs::path, std::uintmax_t>> printThis = sort_memo(entries);



        std::cout << std::endl;
        std::cout << "Largest Above ^^^^^^^^^^^^^^^^^^^^^" << std::endl;
        std::cout << "Tree Below vvvvvvvvvvvvvvvvvvvvvvv" << std::endl;
        std::cout << std::endl;


        for (const auto& [key, value] : printThis) {

            std::string formatVal = display_correctly(static_cast<std::uintmax_t>(value));
            std::cout << key.string() << formatVal << std::endl;

        }

        std::cout << "Press enter to close...";
        std::cin.get();
        return 0;

    }

    catch (const fs::filesystem_error& e) {
        //std::cout << e.what() << std::endl;
        return 0;
    }

}
