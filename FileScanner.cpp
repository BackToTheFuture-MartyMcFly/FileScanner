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
    if (!fs::exists(dir))
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
    if (!fileMap.contains(extStr))
        return std::nullopt;

    return fileMap.at(extStr);
}

int main(int argc, char** argv) {
    fs::path path = (argc > 1) ? argv[1] : ".";

    auto fileMap = buildFileMap(path);

    if (fileMap.has_value()) {
        printReport(fileMap.value());

        auto logVec = getFilesByExtension(fileMap.value(), "log");
        if (logVec.has_value()) {
            std::cout << std::endl << "Logs files: ";
            for (auto logFileName : logVec.value()) {
                std::cout << logFileName << "; ";
            }
        }
    }
    std::cout << std::endl;

	return 0;
}