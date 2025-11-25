#include "logger.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <thread>
#include <utility>

#include <windows.h>

#include "errors.hpp"
#include "files.hpp"

namespace TolCat {

    std::string getTimestamp() {
        const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::string timestamp = std::format(
                "{0:%H:%M:%S}",
                std::chrono::time_point_cast<std::chrono::milliseconds>(now)
        );
        return timestamp;
    }

    void ILoggerOutput::logInfo(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) {}

    void ILoggerOutput::logWarn(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) {}

    void ILoggerOutput::logError(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) {}

    void ILoggerOutput::logNeutral(const std::string &timestamp, std::string_view fmt, std::format_args args) {}

    void ILoggerOutput::logDebug(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) {}

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
            if (const std::filesystem::path& entryPath = entry.path(); entryPath.has_extension()) {
                if (entryPath.extension() == ".log") {
                    logs.insert(entryPath);
                }
            }
        }

        // in alphabetical order, oldest files are first
        if (size_t size = logs.size(); size > 10) {
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

    void LoggerFileOutput::logNeutral(const std::string &timestamp, const std::string_view fmt, const std::format_args args) {
        this->logFileStream << "[" << timestamp << "] " << std::vformat(fmt, args) << std::endl;
    }

    void LoggerFileOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] " << std::vformat(fmt, args) << std::endl;
    }

    void LoggerFileOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] WARNING: " << std::vformat(fmt, args) << std::endl;
    }

    void LoggerFileOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] ERROR: " << std::vformat(fmt, args) << std::endl;
    }

    void LoggerFileOutput::logDebug(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->logFileStream << "[" << timestamp << "] [" << nameSection << "] DEBUG: " << std::vformat(fmt, args) << std::endl;
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

    void LoggerConsoleOutput::logNeutral(const std::string &timestamp, const std::string_view fmt, const std::format_args args) {
        this->conOutStream
            << kAnsiWhite << "["
            << kAnsiGreen << timestamp
            << kAnsiWhite << "] "
            << std::vformat(fmt, args)
            << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logInfo(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->conOutStream
            << kAnsiWhite << "["
            << kAnsiGreen << timestamp
            << kAnsiWhite << "] ["
            << kAnsiCyan << nameSection
            << kAnsiWhite << "] "
            << std::vformat(fmt, args)
            << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logWarn(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->conOutStream
                << kAnsiYellow << "["
                << timestamp
                << "] ["
                << nameSection
                << "] WARNING: "
                << std::vformat(fmt, args)
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logError(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->conOutStream
                << kAnsiRed << "["
                << timestamp
                << "] ["
                << nameSection
                << "] ERROR: "
                << std::vformat(fmt, args)
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::logDebug(const std::string &timestamp, const std::string &nameSection, const std::string_view fmt, const std::format_args args) {
        this->conOutStream
                << kAnsiBlue << "["
                << timestamp
                << "] ["
                << nameSection
                << "] ERROR: "
                << std::vformat(fmt, args)
                << kAnsiReset << std::endl;
    }

    void LoggerConsoleOutput::flushStream() {
        this->conOutStream.flush();
    }

    void LoggerConsoleOutput::makeGrey() {
        this->conOutStream << kAnsiGrey;
    }

    std::map<const char *, std::unique_ptr<ILoggerOutput>> Logger::s_loggerOutputs;

    std::thread Logger::s_workerThread;
    std::condition_variable Logger::s_condition;
    std::atomic<bool> Logger::s_exitCondition;
    std::atomic<std::size_t> Logger::s_jobsPending;
    std::queue<std::function<void ()>> Logger::s_jobQueue;
    std::mutex Logger::s_mutex;

    void Logger::neutralFormat(std::string_view fmt, std::format_args args) {
        std::string timestamp = getTimestamp();
        for (auto &val: s_loggerOutputs | std::views::values) {
            std::function<void()> f = std::bind(&TolCat::ILoggerOutput::logNeutral, val.get(), timestamp, fmt, args);
            queueJob(std::move(f));
        }
    }

    void Logger::infoFormat(std::string_view fmt, std::format_args args) {
        std::string timestamp = getTimestamp();
        for (auto &val: s_loggerOutputs | std::views::values) {
            std::function<void()> f = std::bind(&TolCat::ILoggerOutput::logInfo, val.get(), timestamp, std::ref(this->_sourceName), fmt, args);
            queueJob(std::move(f));
        }
    }

    void Logger::warnFormat(std::string_view fmt, std::format_args args) {
        std::string timestamp = getTimestamp();
        for (auto &val: s_loggerOutputs | std::views::values) {
            std::function<void()> f = std::bind(&TolCat::ILoggerOutput::logWarn, val.get(), timestamp, std::ref(this->_sourceName), fmt, args);
            queueJob(std::move(f));
        }
    }

    void Logger::errorFormat(std::string_view fmt, std::format_args args) {
        std::string timestamp = getTimestamp();
        for (auto &val: s_loggerOutputs | std::views::values) {
            std::function<void()> f = std::bind(&TolCat::ILoggerOutput::logError, val.get(), timestamp, std::ref(this->_sourceName), fmt, args);
            queueJob(std::move(f));
        }
    }

    void Logger::debugFormat(std::string_view fmt, std::format_args args) {
        std::string timestamp = getTimestamp();
        for (auto &val: s_loggerOutputs | std::views::values) {
            std::function<void()> f = std::bind(&TolCat::ILoggerOutput::logError, val.get(), timestamp, std::ref(this->_sourceName), fmt, args);
            queueJob(std::move(f));
        }
    }

    void Logger::flushStreams() {
        for (const auto &val: Logger::s_loggerOutputs | std::views::values) {
            val->flushStream();
        }
    }

    void Logger::queueJob(std::function<void()> f) {
        {
            std::lock_guard lock(s_mutex);
            s_jobQueue.push(std::move(f));
            ++s_jobsPending;
        }
        s_condition.notify_one();
    }

    void Logger::worker() {
        while (!s_exitCondition) {
            work();
        }
    }

    void Logger::work() {
        std::function<void()> f;

        {
            std::unique_lock lock(s_mutex);

            while (s_jobsPending == 0) {
                s_condition.wait(lock);
            }

            if (s_exitCondition) {
                return;
            }

            f = std::move(s_jobQueue.front());
            s_jobQueue.pop();
            --s_jobsPending;
        }

        f();
    }

    Logger::Logger(std::string sourceName) {
        this->_sourceName = std::move(sourceName);
    }

    void Logger::beginLog() {
        s_exitCondition = false;
        s_workerThread = std::thread(Logger::worker);
    }

    void Logger::endLog() {
        s_exitCondition = true;
        s_workerThread.join();
    }
} // namespace TolCat