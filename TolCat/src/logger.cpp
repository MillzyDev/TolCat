#include "logger.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <utility>

#include <windows.h>

#include "errors.hpp"
#include "files.hpp"

namespace TolCat {

    std::string getTimestamp() {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::string timestamp = std::format(
                "{0:%H:%M:%S}",
                std::chrono::time_point_cast<std::chrono::milliseconds>(now)
        );
        return timestamp;
    }

    void ILoggerOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::logNeutral(const std::string &timestamp, const std::string &messageSection) {}

    void ILoggerOutput::logDebug(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {}

    void ILoggerOutput::flushStream() {}

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

    LoggerFileOutput::~LoggerFileOutput() {
        this->logFileStream.close();
    }

    void LoggerFileOutput::logNeutral(const std::string &timestamp, const std::string &messageSection) {
        this->logFileStream << "[" << timestamp << "] " << messageSection << std::endl;
    }

    void LoggerFileOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] " << messageSection << std::endl;
    }

    void LoggerFileOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] WARNING: " << messageSection << std::endl;
    }

    void LoggerFileOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] ERROR: " << messageSection << std::endl;
    }

    void LoggerFileOutput::logDebug(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] DEBUG: " << messageSection << std::endl;
    }

    void LoggerFileOutput::flushStream() {
        this->logFileStream.flush();
    }

    LoggerConsoleOutput::LoggerConsoleOutput() {
        SetLastError(0);

        if (!AllocConsole()) { // allocate console
            // failing here likely means someone is trying to do something naughty
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

        // Allow the console to work with ANSI escape codes
        if (!SetConsoleMode(consoleOutHandle, (consoleOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))) {
            ERROR_ABORT(GetLastError()); // noreturn
        }

        if (!SetConsoleTitleA(MOD_NAME " v" MOD_VERSION " Debug Console")) {
            ERROR_ABORT(GetLastError());
        }

        this->conOutStream.open("CONOUT$");
    }

    LoggerConsoleOutput::~LoggerConsoleOutput() {
        this->conOutStream.close();
    }

    void LoggerConsoleOutput::logNeutral(const std::string &timestamp, const std::string &messageSection) {
        this->conOutStream
            << kAnsiWhite << "["
            << kAnsiGreen << timestamp
            << kAnsiWhite << "] "
            << messageSection
            << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->conOutStream
            << kAnsiWhite << "["
            << kAnsiGreen << timestamp
            << kAnsiWhite << "] ["
            << kAnsiCyan << nameSection
            << kAnsiWhite << "] "
            << messageSection
            << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->conOutStream
                << kAnsiYellow << "["
                << timestamp
                << "] ["
                << nameSection
                << "] WARNING: "
                << messageSection
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->conOutStream
                << kAnsiRed << "["
                << timestamp
                << "] ["
                << nameSection
                << "] ERROR: "
                << messageSection
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logDebug(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) {
        this->conOutStream
                << kAnsiBlue << "["
                << timestamp
                << "] ["
                << nameSection
                << "] ERROR: "
                << messageSection
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::flushStream() {
        this->conOutStream.flush();
    }

    void LoggerConsoleOutput::makeGrey() {
        this->conOutStream << kAnsiGrey;
    }

    std::map<const char *, std::unique_ptr<ILoggerOutput>> Logger::loggerOutputs;

    void Logger::logNeutral(const std::string &messageSection) {
        std::string timestamp = getTimestamp();
        for (auto &output : loggerOutputs) {
            output.second->logNeutral(timestamp, messageSection);
        }
    }

    void Logger::logInfo(const std::string& nameSection, const std::string &messageSection) {
        std::string timestamp = getTimestamp();
        for (auto &output : loggerOutputs) {
            output.second->logInfo(timestamp, nameSection, messageSection);
        }
    }

    void Logger::logWarn(const std::string &nameSection, const std::string &messageSection) {
        std::string timestamp = getTimestamp();
        for (auto &output : loggerOutputs) {
            output.second->logWarn(timestamp, nameSection, messageSection);
        }
    }

    void Logger::logError(const std::string &nameSection, const std::string &messageSection) {
        std::string timestamp = getTimestamp();
        for (auto &output : loggerOutputs) {
            output.second->logError(timestamp, nameSection, messageSection);
        }
    }

    void Logger::logDebug(const std::string &nameSection, const std::string &messageSection) {
        std::string timestamp = getTimestamp();
        for (auto &output : loggerOutputs) {
            output.second->logDebug(timestamp, nameSection, messageSection);
        }
    }

    void Logger::flushStreams() {
        for (auto & loggerOutput : Logger::loggerOutputs) {
            loggerOutput.second->flushStream();
        }
    }

    Logger::Logger(std::string sourceName) {
        this->sourceName = std::move(sourceName);
    }

} // namespace TolCat