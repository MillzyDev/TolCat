#ifndef PRELOADER_ERROR_HANDLING_H_
#define PRELOADER_ERROR_HANDLING_H_

#include <format>
#include <string>
#include <system_error>

#include <windows.h>

#define PRE_STR2(k) #k
#define PRE_STR(k) PRE_STR2(k)
#define HANDLE_SYSTEM_ERROR(error) handleSystemError(error, __FUNCTION__, PRE_STR(__LINE__))

std::string getWindowsErrorMessage(DWORD error);

inline void handleSystemError(DWORD error, const char *function, const char *line) {
    std::string message = std::format("An error occurred in {0}:{1}\n0x{2:x} | {3}",
                                      function, line, error, getWindowsErrorMessage(error));
    (void)MessageBoxA(nullptr, message.c_str(), "TolCat Preloader Error", MB_ICONERROR | MB_OK);
}

#endif // PRELOADER_ERROR_HANDLING_H_