#include "filename_generator.hpp"
#include <random>
#include <sstream>

namespace nl::utils {

std::string FilenameGenerator::GenerateRandomFilename(const std::string& fileName) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 200);

    std::stringstream filename;
    for (int i = 0; i < 50; ++i) {
        filename << std::hex << dis(gen);
    }
    std::string extension = GetFileExtension(fileName);
    filename << "." << extension;

    return filename.str();
}

std::string FilenameGenerator::GetFileExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos && pos != 0) {
        return filename.substr(pos + 1);
    }
    return "";
}

} // namespace nl::utils
