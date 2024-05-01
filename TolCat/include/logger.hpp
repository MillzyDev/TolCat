#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include "tolcat_config.hpp"

namespace TolCat {
    class TOLCAT_API LoggerOutputInterface {
    public:
        virtual void logInfo(std::string timestamp, std::string nameSection, std::string messageSection);
        virtual void logWarn(std::string timestamp, std::string nameSection, std::string messageSection);
        virtual void logError(std::string timestamp, std::string nameSection, std::string messageSection);
    };

    class TOLCAT_API LoggerFileOutput : public LoggerOutputInterface {
    private:
        std::ofstream logFileStream;
    public:
        explicit LoggerFileOutput(const std::filesystem::path &logsDir);
        void logInfo(std::string timestamp, std::string nameSection, std::string messageSection) override;
        void logWarn(std::string timestamp, std::string nameSection, std::string messageSection) override;
        void logError(std::string timestamp, std::string nameSection, std::string messageSection) override;
    };

    class TOLCAT_API LoggerConsoleOutput : public LoggerOutputInterface {
    private:
        std::ofstream conOutStream;
    public:
        LoggerConsoleOutput();
        void logInfo(std::string timestamp, std::string nameSection, std::string messageSection) override;
        void logWarn(std::string timestamp, std::string nameSection, std::string messageSection) override;
        void logError(std::string timestamp, std::string nameSection, std::string messageSection) override;
    };

    class TOLCAT_API Logger {

    };
}