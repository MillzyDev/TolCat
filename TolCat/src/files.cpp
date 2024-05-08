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
        if (base_dir.has_value()) { // return path if already assigned
            return base_dir.value();
        }

        SetLastError(0);
        char main_process[MAX_PATH];
        if(!GetModuleFileNameA(nullptr, main_process, MAX_PATH)) {
            ERROR_ABORT(GetLastError());
        }

        base_dir = std::filesystem::path(main_process).parent_path(); // get the directory the root process is in
        return base_dir.value();
    }

    // TODO: remove macros for clarity
    SUBDIR_GETTER(getModDir, MOD_DATA)
    SUBDIR_GETTER(getAddonsDir, MOD_DATA / "Addons")
    SUBDIR_GETTER(getUserLibsDir, MOD_DATA / "UserLibs")
    SUBDIR_GETTER(getLogsDir, MOD_DATA / "Logs")

    void createLogsDir() {
        std::error_code errorCode;
        std::filesystem::path logsDir = getLogsDir();

        if (exists(logsDir)) {
            return;
        }

        if(!std::filesystem::create_directories(logsDir, errorCode)) {
            ERROR_ABORT(errorCode.value());
        }
    }

    void createUserLibsDir() {
        std::error_code errorCode;
        std::filesystem::path userLibsDir = getUserLibsDir();

        if (exists(userLibsDir)) {
            return;
        }

        if(!std::filesystem::create_directories(userLibsDir, errorCode)) {
            ERROR_ABORT(errorCode.value());
        }
    }

    void createAddonsDir() {
        std::error_code errorCode;
        std::filesystem::path addonsDir = getAddonsDir();

        if (exists(addonsDir)) {
            return;
        }

        if(!std::filesystem::create_directories(addonsDir, errorCode)) {
            ERROR_ABORT(errorCode.value());
        }
    }
}