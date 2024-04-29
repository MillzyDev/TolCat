#pragma once

#include <filesystem>
#include <fstream>

bool unitySanityClean(const std::filesystem::path &globalGameManagersPath);
bool gameSanityClean(const std::filesystem::path &appInfo);