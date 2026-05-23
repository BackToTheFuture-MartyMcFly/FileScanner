//FileScanner.cpp

#include <iostream>
#include <string>
#include <variant>
#include <memory>
#include <optional>
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace fs = std::filesystem;

std::string_view getExtension(std::string_view filename) {
    size_t dot = filename.find_last_of('.');
    if (dot == std::string_view::npos || dot + 1 >= filename.size()) {
        return "unknown";  
    }
    return filename.substr(dot + 1);
}

using FileMap = std::unordered_map<std::string, std::vector<std::string>>;

std::optional<FileMap> buildFileMap(const fs::path& dir) {
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return std::nullopt;
    FileMap result;
    for (auto dirIT : fs::recursive_directory_iterator(dir)) {
        if (fs::is_regular_file(dirIT)) {
            auto fileName = dirIT.path().string();
            auto ext = getExtension(fileName);
            result[std::string(ext)].push_back(fileName);
        }
    }
    return result;
}

void printReport(const FileMap& fileMap) {
    for (const auto& [key, value] : fileMap) { 
        std::cout << "Extension: " << key << " (" << value.size() << " files)" << std::endl;
        for (const auto& file : value) {
            std::error_code ec;
            auto size = fs::file_size(file, ec);
            if(!ec)
                std::cout << "    " << fs::path(file).filename() << " (" << size << " bytes)" << std::endl;
            else
                std::cout << "  - " << fs::path(file).filename() << " (size unknown)" << std::endl;
        }
    }
}

std::optional<std::vector<std::string>> getFilesByExtension(const FileMap & fileMap, std::string_view ext) {
    auto extStr = std::string(ext);
    if (ext.empty())
        extStr = "unknown";
    if (!fileMap.contains(extStr))  //c++20
        return std::nullopt;

    return fileMap.at(extStr);
}

int main(int argc, char** argv) {
    fs::path path = (argc > 1) ? argv[1] : ".";
    std::string ext = (argc > 2) ? argv[2] : "";

    auto fileMap = buildFileMap(path);

    std::cout << "Current diretory: " << fs::absolute(path) << std::endl << std::endl;

    if (fileMap.has_value()) {
        printReport(fileMap.value());

        if (!ext.empty()) {
            auto filesByExt = getFilesByExtension(fileMap.value(), ext);
            if (filesByExt.has_value()) {
                std::cout << std::endl << "Files with extension " << ext << ":" << std::endl;
                for (auto fileName : filesByExt.value()) {
                    std::cout << fileName << std::endl;
                }
            }
        }
    }
    std::cout << std::endl;

	return 0;
}