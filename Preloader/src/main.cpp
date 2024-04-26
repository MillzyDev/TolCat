/**
 * Preloader is responsible for doing the final checks before performing the TolCat initialisation steps
 * Tolcat is basically just a library with all the modding functions in it.
 * The preloader checks if the unity version and target game are correct, and then loads TolCat, and then the Mod
*/

#if WIN32
#include <windows.h>
#else
#error "Unsupported platform"
// support for linux planned eventually
#endif

#if WIN32
#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, [[maybe_unused]] LPVOID lpvReserved) {

    if (fdwReason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    (void)DisableThreadLibraryCalls(hinstDll);

#else // WIN32
[[maybe_unused]] __attribute__((constructor)) preloader_init() {
#endif // WIN32



#if WIN32
    return TRUE;
#endif // WIN32
}
#pragma clang diagnostic pop