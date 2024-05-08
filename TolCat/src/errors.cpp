#include <sstream>

#include "errors.hpp"

#define MESSAGE_FLAGS (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS)
#define LANG_ID MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)

namespace TolCat::Errors {
    std::string getErrorMessage(DWORD code) {
        LPSTR buffer = nullptr;
        size_t size = FormatMessageA(MESSAGE_FLAGS, nullptr, code, LANG_ID, reinterpret_cast<LPSTR>(&buffer), 0,
                                     nullptr);

        std::string errorMessage = std::string(buffer, size);
        LocalFree(buffer);
        return errorMessage;
    }

    __declspec(noreturn) void abortWithError(DWORD code, const char *function, unsigned long line) {
        std::string message = getErrorMessage(code);

        std::stringstream messageStream;
        messageStream << "An error occurred in " << function << ":" << line << "\n"
                       << "Code: 0x" << std::hex << code << "\n"
                       << message;

        (void) MessageBoxA(nullptr, messageStream.str().c_str(), MOD_NAME "| TolCat Fatal Error", MB_OK | MB_ICONERROR);

        std::abort();
    }
}