#include "sanity_checks.hpp"

#include <fstream>
#include <sstream>
#include <string>

bool unitySanityClean(const std::filesystem::path& globalGameManagersPath) {
    constexpr unsigned int kUnityVersionOffset = 0x00000030;

    std::ifstream globalGameManagers(globalGameManagersPath);
    globalGameManagers.seekg(kUnityVersionOffset);
    std::string unityVersion;
    std::getline(globalGameManagers, unityVersion, '\0');

    std::string unityMajor, unityMinor, unityPatch;
    std::stringstream unityVersionStream(unityVersion);

    std::getline(unityVersionStream, unityMajor, '.');
    std::getline(unityVersionStream, unityMinor, '.');
    std::getline(unityVersionStream, unityPatch, '.');

    bool isClean = true;

#ifndef UNITY_MAJOR_AGNOSTIC
    isClean &= (unityMajor == UNITY_MAJOR);
#endif
#ifndef UNITY_MINOR_AGNOSTIC
    isClean &= (unityMinor == UNITY_MINOR);
#endif
#ifndef UNITY_MAJOR_AGNOSTIC
    isClean &= (unityPatch == UNITY_PATCH);
#endif

    return isClean;
}

bool gameSanityClean(const std::filesystem::path& appInfoPath) {
    std::ifstream appInfo(appInfoPath);
    bool isClean = true;

#ifndef GAME_AGNOSTIC
    std::string companyName;
    std::getline(appInfo, companyName, '\n');
    isClean &= (companyName == GAME_COMPANY);

    std::string gameName;
    std::getline(appInfo, gameName, '\n');
    isClean &= (gameName == GAME_NAME);
#endif

    return isClean;
}
