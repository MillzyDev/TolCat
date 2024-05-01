#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <type_traits>

#include "tolcat_config.hpp"

namespace TolCat {
    class TOLCAT_API LoggerOutputInterface {
    public:
        virtual void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
    };

    class TOLCAT_API LoggerFileOutput : public LoggerOutputInterface {
    private:
        std::ofstream logFileStream;
    public:
        explicit LoggerFileOutput(const std::filesystem::path &logsDir);
        void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
    };

    class TOLCAT_API LoggerConsoleOutput : public LoggerOutputInterface {
    private:
        std::ofstream conOutStream;
    public:
        LoggerConsoleOutput();
        void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
    };

    class TOLCAT_API Logger {
    private:
        std::vector<LoggerOutputInterface> loggerOutputs;

    public:
        template<class TLoggerOutput>
        inline void addLoggerOutput(TLoggerOutput loggerOutput) {
            static_assert(std::is_base_of_v<LoggerOutputInterface, TLoggerOutput>, "Class of TLoggerOutput must derive from LoggerOutputInterface");
            this->loggerOutputs.push_back(loggerOutput);
        }
    };
}