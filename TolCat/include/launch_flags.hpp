#ifndef TOLCAT_LAUNCH_FLAGS_H_
#define TOLCAT_LAUNCH_FLAGS_H_

#include <map>
#include <string>

enum TolCatLaunchArgs : unsigned int {
    NONE [[maybe_unused]] = 0,
    FORCE_LOAD = 1 << 0, //    --force-load          Skips all sanity checks performed by the preloader
    DEBUG_CONSOLE = 1 << 1, // --debug-console       Creates a console log window, on by default if debug build
    NO_ADDONS = 1 << 2, //     --no-addons           Disables the addon system

};

inline TolCatLaunchArgs operator |(TolCatLaunchArgs lhs, TolCatLaunchArgs rhs) {
    return static_cast<TolCatLaunchArgs>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline TolCatLaunchArgs operator &(TolCatLaunchArgs lhs, TolCatLaunchArgs rhs) {
    return static_cast<TolCatLaunchArgs>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

inline TolCatLaunchArgs &operator |= (TolCatLaunchArgs &lhs, TolCatLaunchArgs rhs) {
    return lhs = lhs | rhs;
}

inline bool hasLaunchArg(TolCatLaunchArgs subject, TolCatLaunchArgs flag) {
    return (subject & flag) == flag;
}

#endif // TOLCAT_LAUNCH_FLAGS_H_