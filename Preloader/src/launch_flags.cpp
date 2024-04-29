#include "launch_flags.hpp"

std::map<std::wstring, TolCatLaunchArgs> launchArgsReference = {
        { L"--force-load", TolCatLaunchArgs::FORCE_LOAD },
        { L"--debug-console", TolCatLaunchArgs::DEBUG_CONSOLE },
        { L"--no-addons", TolCatLaunchArgs::NO_ADDONS},
};