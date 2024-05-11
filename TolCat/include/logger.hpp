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
#include <type_traits>

namespace TolCat {
    TOLCAT_API std::string getTimestamp();

    class TOLCAT_API ILoggerOutput {
    public:
        virtual void logNeutral(const std::string &timestamp, const std::string &messageSection);
        virtual void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);

        virtual ~ILoggerOutput() = default;
    };

    class TOLCAT_API LoggerFileOutput final : public ILoggerOutput {
    private:
        std::ofstream logFileStream;

    public:
        explicit LoggerFileOutput(const std::filesystem::path &logsDir);
        void logNeutral(const std::string &timestamp, const std::string &messageSection) override;
        void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
    };

    class TOLCAT_API LoggerConsoleOutput final : public ILoggerOutput {
    private:
        std::ofstream conOutStream;

        static constexpr const char *kAnsiReset = "\x1b[0m";
        static constexpr const char *kAnsiRed = "\x1b[31m";
        static constexpr const char *kAnsiGreen = "\x1b[32m";
        static constexpr const char *kAnsiWhite = "\x1b[37m";
        static constexpr const char *kAnsiGrey = "\x1b[90m";
        static constexpr const char *kAnsiYellow = "\x1b[93m";
        static constexpr const char *kAnsiCyan = "\x1b[96m";

    public:
        LoggerConsoleOutput();

        void logNeutral(const std::string &timestamp, const std::string &messageSection) override;
        void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;

        void makeGrey();
    };

    class TOLCAT_API Logger final {
    private:
        static std::map<const char *,std::unique_ptr<ILoggerOutput>> loggerOutputs;

        std::string sourceName;

        static void logNeutral(const std::string &messageSection);
        static void logInfo(const std::string &nameSection, const std::string &messageSection);
        static void logWarn(const std::string &nameSection, const std::string &messageSection);
        static void logError(const std::string &nameSection, const std::string &messageSection);

    public:
        explicit Logger(std::string sourceName);

        template<class TLoggerOutput>
        inline static void addLoggerOutput(std::unique_ptr<TLoggerOutput> loggerOutput) {
            static_assert(std::is_base_of_v<ILoggerOutput, TLoggerOutput>);
            loggerOutputs.emplace(std::make_pair(typeid(TLoggerOutput).name(), std::move(loggerOutput)));
        }

        template<typename... TArgs>
        inline static void neutral(std::format_string<TArgs...> format, TArgs &&...args) {
            logNeutral(std::vformat(format.get(), std::make_format_args(args...)));
        }

        template<typename... Args>
        inline void info(std::format_string<Args...> format, Args &&...args) {
            logInfo(
                this->sourceName,
                std::vformat(format.get(), std::make_format_args(args...))
            );
        }

        template<typename... Args>
        inline void warn(std::format_string<Args...> format, Args &&...args) {
            logWarn(
                    this->sourceName,
                    std::vformat(format.get(), std::make_format_args(args...))
            );
        }

        template<typename... Args>
        inline void error(std::format_string<Args...> format, Args &&...args) {
            logError(
                    this->sourceName,
                    std::vformat(format.get(), std::make_format_args(args...))
            );
        }
    };

    extern Logger tolCatLogger_;
}

#endif // TOLCAT_LOGGER_H_