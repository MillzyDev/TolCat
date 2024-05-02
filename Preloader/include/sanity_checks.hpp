#ifndef PRELOADER_SANITY_CHECKS_H_
#define PRELOADER_SANITY_CHECKS_H_

#include <filesystem>

bool unitySanityClean(const std::filesystem::path &globalGameManagersPath);
bool gameSanityClean(const std::filesystem::path &appInfo);

#endif // PRELOADER_SANITY_CHECKS_H_