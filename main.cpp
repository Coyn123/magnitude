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
