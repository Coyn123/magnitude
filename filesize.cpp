#include "filesize.h"
#include <iostream>

std::uintmax_t total_size_of(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes) {
    std::error_code ec;
    std::uintmax_t running_size = 0;

    try {

        auto it = fs::directory_iterator(path);

        while (it != fs::directory_iterator{}) {
            const auto& file = *it;

            //is the file name in excludes? (filesize.h)
            if( excludes.count(file.path().filename().string()) ) {
                it.increment(ec);
                if(!ec) continue; else return memo_sizes[path] = running_size;
            }

            if ( file.is_regular_file() ) { running_size += file.file_size(); }
            if ( file.is_directory() ) { std::uintmax_t recur = total_size_of(file.path(), memo_sizes); running_size += recur; }
            it.increment(ec);
            if(!ec) continue; else return memo_sizes[path] = running_size;
        }

        return memo_sizes[path] = running_size;
    }

    catch (const fs::filesystem_error& e) {
        //std::cout << e.what() << std::endl;
        return memo_sizes[path] = running_size;
    }
}

BiggestEntry biggest_child_of(fs::path const& path, std::unordered_map<fs::path, std::uintmax_t>& memo_sizes) {

    std::uintmax_t running_largest = 0;
    std::error_code ec;
    fs::path largest_file_path;

    //check filesize.h for struct
    BiggestEntry curEntry;

    try {

        auto it = fs::directory_iterator(path);

        while ( it != fs::directory_iterator{} ) {

            std::uintmax_t curSize = 0;
            const auto& curFile = *it;

            //is the file name in excludes? (filesize.h)
            if( excludes.count(curFile.path().filename().string()) ) {
                it.increment(ec);
                if(!ec) continue; else return curEntry;
            }

            if(curFile.is_directory()) curSize = memo_sizes[curFile.path()];
            if(curFile.is_regular_file()) curSize = curFile.file_size();
            if (curSize > running_largest) {
                running_largest = curSize;
                largest_file_path = curFile;
                curEntry.path_to_biggest = largest_file_path;
                curEntry.size_of_biggest = running_largest;
            }

            it.increment(ec);
            if(!ec) continue; else return curEntry;
        }
        return curEntry;

    }
    catch (const fs::filesystem_error& e) {
        //std::cout << e.what() << std::endl;
        return curEntry;
    }

}
