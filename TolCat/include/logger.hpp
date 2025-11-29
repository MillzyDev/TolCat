#ifndef TOLCAT_LOGGER_H_
#define TOLCAT_LOGGER_H_

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "launch_flags.hpp"
#include "tolcat_config.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/wincolor_sink.h"

namespace TolCat {
    constexpr auto kLatestLogName = "_Latest.log";
    constexpr std::uint32_t kMaxArchivedLogs = 10;

    /**
     * Lazily gets the logger for TolCat (unconfigured by default; no sinks)
     * @note This should NOT be used for creating your own logger.
     * @return A shared pointer to the initialised logger
     */
    TOLCAT_API std::shared_ptr<spdlog::logger> getLogger();

    /**
     * Renames the latest log file to be respective of when it was last written to.
     * @param logDirectory The logs directory containing the latest
     * @param log The file name of the log to archive
     */
    TOLCAT_API void archiveLog(const std::filesystem::path &logDirectory, std::string_view log = kLatestLogName);

    /**
     * Deletes the oldest log files in a directory until the youngest of a specified amount remain
     * @param logDirectory The directory containing log files to delete
     * @param maxLogs Maximum number of log files permitted in the directory
     */
    TOLCAT_API void deleteOldLogs(const std::filesystem::path &logDirectory, std::uint32_t maxLogs = kMaxArchivedLogs);

    /**
     * Allocates a console for this process
     */
    TOLCAT_API void createConsole();

    /**
     * Binds stdout to the allocated console for this process
     */
    TOLCAT_API void bindConsole();

    /**
     * Helper class for creating logger sinks
     */
    class TOLCAT_API SinkMaker {
    private:
        std::filesystem::path logDirectory_;
        std::string_view logFileName_;

    public:
        explicit SinkMaker(const std::filesystem::path &logDirectory, std::string_view logFileName = kLatestLogName);

        /**
         * Creates a logger sink instance for a stream to a log file
         * @return Shared pointer instance to the created sink
         */
        std::shared_ptr<spdlog::sinks::basic_file_sink_st> createFileSink() const;

        /**
         * Creates a logger sink instance for a stream to CONOUT$
         * @return Shared pointer instance to the created sink
         */
        std::shared_ptr<spdlog::sinks::stdout_color_sink_st> createConsoleSink() const;
    };
} // TolCat

#endif // TOLCAT_LOGGER_H_