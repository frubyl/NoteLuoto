#include "file_manager.hpp"
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace nl::utils {

const std::string FileManager::directory_ = "attachments/";

std::string FileManager::SaveFile(userver::server::http::FormDataArg& file) {
    auto fileName = !file.filename ? "" : file.filename.value();
    std::string newFileName = FilenameGenerator::GenerateRandomFilename(fileName);
    std::string path = directory_ + newFileName;

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile.is_open()) {
        LOG_ERROR() << "Failed to open file for writing";
        throw std::runtime_error("Failed to open file for writing");
    }
    outFile << file.value;
    if (outFile.fail()) {
        LOG_ERROR() << "Error writing to file";
        throw std::runtime_error("Error writing to file");
    }
    outFile.close();
    return newFileName;
}

std::string FileManager::ReadFile(const std::string& fileName) {
    std::string path = directory_ + fileName;
    std::ifstream inFile(path, std::ios::binary);
    if (!inFile.is_open()) {
        LOG_ERROR() << "Failed to open file for reading";
        throw std::runtime_error("Failed to open file for reading");
    }
    
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    if (inFile.bad()) {
        LOG_ERROR() << "Error reading from file";
        throw std::runtime_error("Error reading from file");
    }
    return buffer.str();
}

void FileManager::DeleteFile(const std::string& fileName) {
    std::string path = directory_ + fileName;
    if (!std::filesystem::remove(path)) {
        LOG_ERROR() << "Failed to delete file: " << path;
        throw std::runtime_error("Failed to delete file");
    }
}

} // namespace nl::utils
