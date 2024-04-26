#include <windows.h>
#include <system_error>
#include <format>
#include <filesystem>

#define PRE_STR2(k) #k
#define PRE_STR(k) PRE_STR2(k)
#define HANDLE_SYSTEM_ERROR(error) handleSystemError(error, __FUNCTION__, PRE_STR(__LINE__))

extern void loadExports(); // proxy.cpp

void handleSystemError(DWORD error, const char *function, const char *line) {
    auto errorCode = std::error_code(static_cast<int>(error), std::system_category());
    std::string message = std::format("And error occurred in {0}:{1}\n0x{2:x} | {3}",
                                          function, line, errorCode.value(), errorCode.message());
    (void)MessageBoxA(nullptr, message.c_str(), "TolCat Proxy Error", MB_ICONERROR | MB_OK);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, [[maybe_unused]] LPVOID lpvReserved) {
    if (fdwReason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    (void)DisableThreadLibraryCalls(hinstDll); // disables DLL_THREAD_ATTACH/DETACH events

    loadExports(); // load original functions

    // Get main exe name
    char mainModuleName[MAX_PATH];
    DWORD error = GetModuleFileNameA(nullptr, mainModuleName, sizeof(mainModuleName));
    if (error) {
        HANDLE_SYSTEM_ERROR(error);
        return FALSE;
    }

    // Check if data exists folder
    std::filesystem::path mainModulePath(mainModuleName);
    std::filesystem::path dataFolder = mainModulePath.parent_path() / mainModulePath.stem().string().append("_Data");
    if (!exists(dataFolder)) {
        return TRUE;
    }

    // Load Preloader


    return TRUE;
}
#pragma clang diagnostic pop