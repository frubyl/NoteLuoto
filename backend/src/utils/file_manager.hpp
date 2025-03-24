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
#include <userver/server/http/form_data_arg.hpp>

namespace nl::utils {

class FileManager {
public:
  static std::string SaveFile(userver::server::http::FormDataArg& file);
  static std::string ReadFile(const std::string& fileName);
  static void DeleteFile(const std::string& fileName);
private:
  static const std::string directory_;
};

} // namespace nl::utils