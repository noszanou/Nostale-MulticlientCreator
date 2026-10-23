#pragma once
#include <filesystem>
#include <optional>
#include <string>

namespace ncc::platform
{

[[nodiscard]] std::optional<std::string> fileVersion(const std::filesystem::path& file);

}
