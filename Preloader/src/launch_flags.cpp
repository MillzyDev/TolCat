#include "launch_flags.hpp"

std::map<std::wstring, TolCatLaunchArgs> launchArgsReference = {
        { L"--force-load", TolCatLaunchArgs::kForceLoad },
        { L"--debug-console", TolCatLaunchArgs::kDebugConsole },
        { L"--no-addons", TolCatLaunchArgs::kNoAddons},
};