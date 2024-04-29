#include <windows.h>
#include <string>
#include <map>

#include <format>
#include "command_line.hpp"

TolCatLaunchArgs getLaunchArgs() {
    LPWSTR commandLineStr = GetCommandLineW();
    int argc;
    LPWSTR *argv = CommandLineToArgvW(commandLineStr, &argc);

    MessageBoxW(nullptr, commandLineStr, L"command line", MB_OK);

    TolCatLaunchArgs flags = TolCatLaunchArgs::NONE;
    for (int index = 1; index < argc; index++) {
        auto it = launchArgsReference.find(std::wstring(argv[index]));
        if (it != launchArgsReference.end()) {
            flags |= it->second;
            MessageBoxA(nullptr, std::format("Has flag {:b}", (unsigned int)it->second).c_str(), "aaa", MB_OK);
        }
    }

    return flags;
}