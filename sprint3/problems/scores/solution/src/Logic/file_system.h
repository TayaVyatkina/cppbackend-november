#pragma once
#include <filesystem>

namespace userFileSystem {

bool IsSubPath(std::filesystem::path path, std::filesystem::path base);

}