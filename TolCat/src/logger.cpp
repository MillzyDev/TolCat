#include "logger.hpp"

#include <chrono>
#include <format>
#include <set>

#include <windows.h>

#include "errors.hpp"
#include "files.hpp"

namespace TolCat {

    void ILoggerOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    LoggerFileOutput::LoggerFileOutput(const std::filesystem::path &logsDir) {
        std::error_code error;

        std::filesystem::path latestLog = logsDir / "_Latest.log";

        if (exists(latestLog)) {
            std::filesystem::file_time_type lastWrite = std::filesystem::last_write_time(latestLog, error);
            ERROR_ABORT_UNLESS(error.value())

            std::string previousLogName =
                    std::format(MOD_NAME "_{0:%Y-%m-%d_%H-%M-%S}.log",
                        std::chrono::time_point_cast<std::chrono::milliseconds>(lastWrite)
                    );

            std::filesystem::path previousLog = logsDir / previousLogName;
            std::filesystem::rename(latestLog, previousLog, error);

            ERROR_ABORT_UNLESS(error.value())
        }

        // count and sort files
        auto it = std::filesystem::directory_iterator(logsDir);
        std::set<std::filesystem::path> logs;
        for (const std::filesystem::directory_entry& entry : it) {
            const std::filesystem::path& entryPath = entry.path();

            if (entryPath.has_extension()) {
                if (entryPath.extension() == ".log") {
                    logs.insert(entryPath);
                }
            }
        }

        // in alphabetical order, oldest files are first
        size_t size = logs.size();
        if (size > 10) {
            size_t deleteCount = size - 10;
            for (const std::filesystem::path& oldestLog : logs) {
                if (!deleteCount) {
                    break;
                }
                deleteCount--;

                if (!exists(oldestLog)) {
                    continue;
                }

                if(!std::filesystem::remove(oldestLog, error)) {
                    ERROR_ABORT(error.value());
                }
            }
        }

        this->logFileStream = std::ofstream(latestLog);

    }

    void LoggerFileOutput::logInfo(const std::string& timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    void LoggerFileOutput::logWarn(const std::string& timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    void LoggerFileOutput::logError(const std::string& timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    LoggerConsoleOutput::LoggerConsoleOutput() {
        SetLastError(0);

        if (!AllocConsole()) {
            ERROR_ABORT(GetLastError()); // noreturn
        }

        HANDLE consoleOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (consoleOutHandle == INVALID_HANDLE_VALUE) {
            ERROR_ABORT(GetLastError()); // noreturn
        }

        DWORD consoleOutMode;
        if (!GetConsoleMode(consoleOutHandle, &consoleOutMode)) {
            ERROR_ABORT(GetLastError()); // noreturn
        }

        if (!SetConsoleMode(consoleOutHandle, (consoleOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))) {
            ERROR_ABORT(GetLastError()); // noreturn
        }

        this->conOutStream.open("CONOUT$");
    }

    void LoggerConsoleOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    void LoggerConsoleOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    void LoggerConsoleOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {

    }

    std::vector<ILoggerOutput> Logger::loggerOutputs;

} // namespace TolCat