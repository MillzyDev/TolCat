#include <filesystem>

#include <windows.h>

#include "command_line.hpp"
#include "error_handling.hpp"
#include "sanity_checks.hpp"

BOOL loadTolCat(TolCatLaunchArgs launchArgs) {
    char mainModuleName[MAX_PATH];
    DWORD dataLength = GetModuleFileNameA(nullptr, mainModuleName, sizeof(mainModuleName));
    if (!dataLength) {
        HANDLE_SYSTEM_ERROR(GetLastError());
        return FALSE;
    }

    std::filesystem::path mainModulePath(mainModuleName);
    std::filesystem::path dataFolder = mainModulePath.parent_path() / mainModulePath.stem().string().append("_Data");

    bool isClean = true;
    if (!hasLaunchArg(launchArgs, TolCatLaunchArgs::FORCE_LOAD)) {
        isClean = unitySanityClean(dataFolder / "globalgamemanagers") && gameSanityClean(dataFolder / "app.info");
    }

    if (isClean) {
        std::filesystem::path tolCatPath = mainModulePath.parent_path() / MOD_NAME "_Data" / "TolCat.dll";
        HMODULE tolCatHandle = LoadLibraryExW(tolCatPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

        if (!tolCatHandle) {
            HANDLE_SYSTEM_ERROR(GetLastError());
            return FALSE;
        }

        auto launchTolCat = reinterpret_cast<void (*)(TolCatLaunchArgs)>(GetProcAddress(tolCatHandle, "launchTolCat"));
        if (launchTolCat) {
            launchTolCat(launchArgs);
            return TRUE;
        }

        HANDLE_SYSTEM_ERROR(GetLastError());
        return FALSE;
    }

    return TRUE;
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
    return loadTolCat(launchArgs);
}
#pragma clang diagnostic pop