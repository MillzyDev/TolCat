#include "command_line.hpp"

#include <format>
#include <string>
#include <map>

#include <windows.h>

TolCatLaunchArgs getLaunchArgs() {
    LPWSTR commandLineStr = GetCommandLineW();
    int argc;
    LPWSTR *argv = CommandLineToArgvW(commandLineStr, &argc);

    TolCatLaunchArgs flags = TolCatLaunchArgs::kNone;
    for (int index = 1; index < argc; index++) {
        auto it = launchArgsReference.find(std::wstring(argv[index]));
        if (it != launchArgsReference.end()) {
            flags |= it->second;
        }
    }

    return flags;
}