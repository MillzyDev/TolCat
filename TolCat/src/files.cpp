#include <optional>
#include <windows.h>

#include "errors.hpp"
#include "files.hpp"

#define SUBDIR_GETTER(name, subdir)                                                 \
std::filesystem::path name() {                                                      \
    static std::filesystem::path dirPath = ::TolCat::Files::getBaseDir() / subdir;  \
    return dirPath;                                                                 \
}

constexpr const char *kModData = MOD_NAME "_Data";

namespace TolCat::Files {
    std::filesystem::path getBaseDir() {
        static std::optional<std::filesystem::path> baseDir;
        if (baseDir.has_value()) { // return path if already assigned
            return baseDir.value();
        }

        SetLastError(0);
        char mainProcess[MAX_PATH];
        if(!GetModuleFileNameA(nullptr, mainProcess, MAX_PATH)) {
            ERROR_ABORT(GetLastError());
        }

        baseDir = std::filesystem::path(mainProcess).parent_path(); // get the directory the root process is in
        return baseDir.value();
    }

    std::filesystem::path getModDir() {
        static std::filesystem::path modDir = getBaseDir() / kModData;
        return modDir;
    }

    std::filesystem::path getAddonsDir() {
        static std::filesystem::path addonsDir = getBaseDir() / kModData / "Addons";
        return addonsDir;
    }

    std::filesystem::path getUserLibsDir() {
        static std::filesystem::path userLibsDir = getBaseDir() / kModData / "UserLibs";
        return userLibsDir;
    }

    std::filesystem::path getLogsDir() {
        static std::filesystem::path logsDir = getBaseDir() / kModData / "Logs";
        return logsDir;
    }

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