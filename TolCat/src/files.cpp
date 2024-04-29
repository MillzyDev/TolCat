#include <optional>
#include <windows.h>

#include "errors.hpp"
#include "files.hpp"

#define SUBDIR_GETTER(name, subdir)                                                 \
std::filesystem::path name() {                                                      \
    static std::filesystem::path dirPath = ::TolCat::Files::getBaseDir() / subdir;  \
    return dirPath;                                                                 \
}

#define MOD_DATA MOD_NAME "_Data"

namespace TolCat::Files {
    std::filesystem::path getBaseDir() {
        static std::optional<std::filesystem::path> base_dir;
        if (base_dir.has_value()) {
            return base_dir.value();
        }

        SetLastError(0);
        char main_process[MAX_PATH];
        GetModuleFileNameA(nullptr, main_process, MAX_PATH);
        ERROR_ABORT_UNLESS(GetLastError())

        base_dir = std::filesystem::path(main_process).parent_path();
        return base_dir.value();
    }

    SUBDIR_GETTER(getModDir, MOD_DATA)
    SUBDIR_GETTER(getAddonsDir, MOD_DATA / "Addons")
    SUBDIR_GETTER(getUserLibsDir, MOD_DATA / "UserLibs")
    SUBDIR_GETTER(getLogsDir, MOD_DATA / "Logs")
}