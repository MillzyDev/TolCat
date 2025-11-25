#ifndef TOLCAT_LOGGER_H_
#define TOLCAT_LOGGER_H_

#include "tolcat_config.hpp"

#include <windows.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>

namespace TolCat {
    TOLCAT_API std::string getTimestamp();

    class TOLCAT_API ILoggerOutput {
    public:
        virtual void logNeutral(const std::string &timestamp, std::string_view fmt, std::format_args args);
        virtual void logInfo(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args);
        virtual void logWarn(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args);
        virtual void logError(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args);
        virtual void logDebug(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args);
        virtual void flushStream();

        virtual ~ILoggerOutput() = default;
    };

    class TOLCAT_API LoggerFileOutput final : public ILoggerOutput {
    private:
        std::ofstream logFileStream;

    public:
        explicit LoggerFileOutput(const std::filesystem::path &logsDir);
        ~LoggerFileOutput() override;

        void logNeutral(const std::string &timestamp, std::string_view fmt, std::format_args args) override;
        void logInfo(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logError(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logDebug(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void flushStream() override;
    };

    class TOLCAT_API LoggerConsoleOutput final : public ILoggerOutput {
    private:
        std::ofstream conOutStream;

        static constexpr auto kAnsiReset = "\x1b[0m";
        static constexpr auto kAnsiRed = "\x1b[31m";
        static constexpr auto kAnsiGreen = "\x1b[32m";
        static constexpr auto kAnsiWhite = "\x1b[37m";
        static constexpr auto kAnsiGrey = "\x1b[90m";
        static constexpr auto kAnsiYellow = "\x1b[93m";
        static constexpr auto kAnsiBlue = "\x1b[94m";
        static constexpr auto kAnsiCyan = "\x1b[96m";

    public:
        LoggerConsoleOutput();
        ~LoggerConsoleOutput() override;

        void logNeutral(const std::string &timestamp, std::string_view fmt, std::format_args args) override;
        void logInfo(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logError(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void logDebug(const std::string &timestamp, const std::string &nameSection, std::string_view fmt, std::format_args args) override;
        void flushStream() override;

        void makeGrey();
    };

    class TOLCAT_API Logger final {
    private:
        static std::map<const char *,std::unique_ptr<ILoggerOutput>> s_loggerOutputs;

        std::string _sourceName;

        static void flushStreams();

        // TODO: Move worker into own class
        static std::thread s_workerThread;
        static std::condition_variable s_condition;
        static std::atomic<bool> s_exitCondition;
        static std::atomic<std::size_t> s_jobsPending;
        static std::queue<std::function<void()>> s_jobQueue;
        static std::mutex s_mutex;

        static void queueJob(std::function<void()> f);
        static void worker();
        static void work();

    public:
        explicit Logger(std::string sourceName);

        static void beginLog();
        static void endLog();

        void neutralFormat(std::string_view fmt, std::format_args args);
        void infoFormat(std::string_view fmt, std::format_args args);
        void warnFormat(std::string_view fmt, std::format_args args);
        void errorFormat(std::string_view fmt, std::format_args args);
        void debugFormat(std::string_view fmt, std::format_args args);

        template<class TLoggerOutput>
        inline static void addLoggerOutput(std::unique_ptr<TLoggerOutput> loggerOutput) {
            static_assert(std::is_base_of_v<ILoggerOutput, TLoggerOutput>);
            s_loggerOutputs.emplace(std::make_pair(typeid(TLoggerOutput).name(), std::move(loggerOutput)));
        }

        template<typename... TArgs>
        inline void neutral(std::format_string<TArgs...> format, TArgs &&...args) {
            neutralFormat(format.get(), std::make_format_args(args...));
        }

        template<typename... Args>
        inline void info(std::format_string<Args...> format, Args &&...args) {
            infoFormat(
                format.get(), std::make_format_args(args...)
            );
        }

        template<typename... Args>
        inline void warn(std::format_string<Args...> format, Args &&...args) {
            warnFormat(
                    format.get(), std::make_format_args(args...)
            );
        }

        template<typename... Args>
        inline void error(std::format_string<Args...> format, Args &&...args) {
            errorFormat(
                    format.get(), std::make_format_args(args...)
            );
        }

        template<typename... Args>
        inline void debug(std::format_string<Args...> format, Args &&...args) {
#ifdef DEBUG_LOGS
            debugFormat(
                format.get(), std::make_format_args(args...)
            );
#endif
        }
    };

    extern Logger tolCatLogger_;
} // TolCat

#endif // TOLCAT_LOGGER_H_