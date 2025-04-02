#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <random>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <userver/logging/log.hpp>
#include "filename_generator.hpp"

namespace nl::utils {

class FilenameGenerator  {
public:
static std::string GenerateRandomFilename(const std::string& filename);

protected:
static std::string GetFileExtension(const std::string& filename);
};

} // namespace nl::utils