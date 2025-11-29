#include <iostream>
#include <memory>
#include <set>

#include "logger.hpp"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>

#include "files.hpp"
#include "launch_flags.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace TolCat {
    std::shared_ptr<spdlog::logger> getLogger() {
        static auto logger = std::make_shared<spdlog::logger>("TolCat");
        return logger;
    }

    void archiveLog(const std::filesystem::path &logDirectory, const std::string_view log) {
        if (const std::filesystem::path logFile = logDirectory / log; exists(logFile)) { // do nothing if no log file
            const std::filesystem::file_time_type lastWrite = std::filesystem::last_write_time(logFile);

            const std::string archiveLogName = // format the log file name
                    std::format(MOD_NAME "_{0:%Y-%m-%d_%H-%M-%S}.log",
                        std::chrono::time_point_cast<std::chrono::milliseconds>(lastWrite)
                    );

            // rename log file to formatted archive name
            const std::filesystem::path previousLog = logDirectory / archiveLogName;
            std::filesystem::rename(logFile, previousLog);
        }
    }

    void deleteOldLogs(const std::filesystem::path &logDirectory, const std::uint32_t maxLogs) {
        // count and sort files
        auto it = std::filesystem::directory_iterator(logDirectory);
        std::set<std::filesystem::path> logs;
        for (const std::filesystem::directory_entry& entry : it) {
            if (const std::filesystem::path& entryPath = entry.path(); entryPath.has_extension()) {
                if (entryPath.extension() == ".log") {
                    logs.insert(entryPath);
                }
            }
        }

        // in alphabetical order, oldest files are first
        if (const size_t size = logs.size(); size > maxLogs) {
            size_t deleteCount = size - 10;
            for (const std::filesystem::path& oldestLog : logs) {
                if (!deleteCount) {
                    break;
                }
                deleteCount--;

                if (!exists(oldestLog)) {
                    continue;
                }

                std::filesystem::remove(oldestLog);
            }
        }
    }

    void createConsole() {
        AllocConsole();
    }

    void bindConsole() {
        // Derived from https://stackoverflow.com/a/25927081

        // Initialise with a clean handle
        FILE *dummy;
        freopen_s(&dummy, "NUL", "r", stdout);

        // Redirect unbuffered stdout to the current stdout handle
        HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (stdHandle == INVALID_HANDLE_VALUE) {
            return;
        }

        const int descriptor = _open_osfhandle(reinterpret_cast<intptr_t>(stdHandle), _O_TEXT);
        if (descriptor == -1) {
            return;
        }

        FILE *file = _fdopen(descriptor, "w");
        if (file == nullptr) {
            return;
        }

        if (_dup2(_fileno(file), _fileno(stdout))) {
            return;
        }

        setvbuf(stdout, nullptr, _IONBF, 0);

        std::wcout.clear();
        std::cout.clear();
    }

    SinkMaker::SinkMaker(const std::filesystem::path &logDirectory, const std::string_view logFileName) {
        this->logDirectory_ = logDirectory;
        this->logFileName_ = logFileName;
    }

    std::shared_ptr<spdlog::sinks::basic_file_sink_st> SinkMaker::createFileSink() const {
        return std::make_shared<spdlog::sinks::basic_file_sink_st>((this->logDirectory_ / this->logFileName_).string());
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    std::shared_ptr<spdlog::sinks::stdout_color_sink_st> SinkMaker::createConsoleSink() const {
        return std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    }
} // TolCat