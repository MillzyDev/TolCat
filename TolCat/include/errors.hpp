#pragma once

#include <string>
#include <type_traits>
#include <windows.h>

#include "tolcat_config.hpp"

#define STR_WRAP(k) STR_WRAP_INNER(k)
#define STR_WRAP_INNER(k) #k

#define ERROR_ABORT(code) ::TolCat::Errors::abortWithError(code, __FUNCTION__, __LINE__)

#define ERROR_ABORT_UNLESS(expr)        \
{                                       \
    DWORD _err = expr;                  \
    if (_err)                           \
        ERROR_ABORT(_err);              \
}

namespace TolCat::Errors {
    [[nodiscard]] TOLCAT_API std::string getErrorMessage(DWORD code);
    TOLCAT_API void abortWithError(DWORD code, const char *function, uint64_t line);
}

#undef ALPHA1_EXPORT