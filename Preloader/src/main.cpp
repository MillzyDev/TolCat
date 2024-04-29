/**
 * Preloader is responsible for doing the final checks before performing the TolCat initialisation steps
 * Tolcat is basically just a library with all the modding functions in it.
 * The preloader checks if the unity version and target game are correct, and then loads TolCat
*/

#if WIN32
#include <windows.h>
#else
#error "Unsupported platform"
// support for linux planned eventually
#endif

#include <filesystem>

#include "command_line.hpp"
#include "error_handling.hpp"
#include "sanity_checks.hpp"

bool loadTolCat(bool requireSanity) {
    char mainModuleName[MAX_PATH];
    DWORD dataLength = GetModuleFileNameA(nullptr, mainModuleName, sizeof(mainModuleName));
    if (!dataLength) {
        HANDLE_SYSTEM_ERROR(GetLastError());
        return false;
    }

    std::filesystem::path mainModulePath(mainModuleName);
    std::filesystem::path dataFolder = mainModulePath.parent_path() / mainModulePath.stem().string().append("_Data");

    bool isClean = true;
    if (requireSanity) {
        isClean = unitySanityClean(dataFolder / "globalgamemanagers") && gameSanityClean(dataFolder / "app.info");
    }

    if (isClean) {
        std::filesystem::path tolCatPath = mainModulePath.parent_path() / MOD_NAME "_Data" / "TolCat.dll";
        HMODULE tolCatHandle = LoadLibraryExW(tolCatPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

        if (!tolCatHandle) {
            HANDLE_SYSTEM_ERROR(GetLastError());
            return false;
        }
    }

    return true;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, [[maybe_unused]] LPVOID lpvReserved) {

    if (fdwReason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    bool success = DisableThreadLibraryCalls(hinstDll);
    if (!success) {
        HANDLE_SYSTEM_ERROR(GetLastError());
    }

    TolCatLaunchArgs launchArgs = getLaunchArgs();
    return loadTolCat(!hasLaunchArg(launchArgs, TolCatLaunchArgs::FORCE_LOAD));

}
#pragma clang diagnostic pop