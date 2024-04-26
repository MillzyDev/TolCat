#include <libloaderapi.h>
#include <sysinfoapi.h>

#include <filesystem>

#define PROXY_FUNCTION(name)                            \
FARPROC orig_##name;                                    \
extern "C" __declspec(dllexport) void WINAPI name() {   \
        orig_##name();                                  \
}

#define PROXY_ADDRESS(module, name) orig_##name = GetProcAddress(module, #name)

PROXY_FUNCTION(GetFileVersionInfoA)
PROXY_FUNCTION(GetFileVersionInfoByHandle)
PROXY_FUNCTION(GetFileVersionInfoExA)
PROXY_FUNCTION(GetFileVersionInfoExW)
PROXY_FUNCTION(GetFileVersionInfoSizeA)
PROXY_FUNCTION(GetFileVersionInfoSizeExA)
PROXY_FUNCTION(GetFileVersionInfoSizeExW)
PROXY_FUNCTION(GetFileVersionInfoSizeW)
PROXY_FUNCTION(GetFileVersionInfoW)
PROXY_FUNCTION(VerFindFileA)
PROXY_FUNCTION(VerFindFileW)
PROXY_FUNCTION(VerInstallFileA)
PROXY_FUNCTION(VerInstallFileW)
PROXY_FUNCTION(VerLanguageNameA)
PROXY_FUNCTION(VerlanguageNameW)
PROXY_FUNCTION(VerQueryValueA)
PROXY_FUNCTION(VerQueryValueW)

HMODULE getOriginalHandle() {
    char system32[MAX_PATH];
    GetSystemDirectoryA(system32, sizeof(system32));
    std::filesystem::path original_dll(system32);
    original_dll /= "version.dll";

    // I think the machine is beyond fucked if it somehow can't find a dll in system32,
    // so I don't think error handling is worth the effort here.
    return LoadLibraryA(original_dll.string().c_str());
}

void loadExports() {
    HMODULE module = getOriginalHandle();
    // tbf the macro spam could be worse
    PROXY_ADDRESS(module, GetFileVersionInfoA);
    PROXY_ADDRESS(module, GetFileVersionInfoByHandle);
    PROXY_ADDRESS(module, GetFileVersionInfoExA);
    PROXY_ADDRESS(module, GetFileVersionInfoExW);
    PROXY_ADDRESS(module, GetFileVersionInfoSizeA);
    PROXY_ADDRESS(module, GetFileVersionInfoSizeExA);
    PROXY_ADDRESS(module, GetFileVersionInfoSizeExW);
    PROXY_ADDRESS(module, GetFileVersionInfoSizeW);
    PROXY_ADDRESS(module, GetFileVersionInfoW);
    PROXY_ADDRESS(module, VerFindFileA);
    PROXY_ADDRESS(module, VerFindFileW);
    PROXY_ADDRESS(module, VerInstallFileA);
    PROXY_ADDRESS(module, VerInstallFileW);
    PROXY_ADDRESS(module, VerLanguageNameA);
    PROXY_ADDRESS(module, VerlanguageNameW);
    PROXY_ADDRESS(module, VerQueryValueA);
    PROXY_ADDRESS(module, VerQueryValueW);
}