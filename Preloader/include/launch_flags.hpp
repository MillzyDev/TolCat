#pragma once

#include <cstdint>
#include <map>
#include <string>

enum TolCatLaunchArgs : uint32_t {
    NONE = 0,
    FORCE_LOAD = 1 << 0, //    --force-load          Skips all sanity checks performed by the preloader
    DEBUG_CONSOLE = 1 << 1, // --debug-console       Creates a console log window, on by default if debug build
    NO_ADDONS = 1 << 2, //     --no-addons           Disables the addon system
    
};

inline TolCatLaunchArgs operator |(TolCatLaunchArgs lhs, TolCatLaunchArgs rhs) {
    return static_cast<TolCatLaunchArgs>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline TolCatLaunchArgs operator &(TolCatLaunchArgs lhs, TolCatLaunchArgs rhs) {
    return static_cast<TolCatLaunchArgs>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

inline TolCatLaunchArgs &operator |= (TolCatLaunchArgs &lhs, TolCatLaunchArgs rhs) {
    return lhs = lhs | rhs;
}

inline bool hasLaunchArg(TolCatLaunchArgs subject, TolCatLaunchArgs flag) {
    return (subject & flag) == flag;
}

extern std::map<std::wstring, TolCatLaunchArgs> launchArgsReference;