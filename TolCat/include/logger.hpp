#ifndef TOLCAT_LOGGER_H_
#define TOLCAT_LOGGER_H_

#include "tolcat_config.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <type_traits>


namespace TolCat {
    class TOLCAT_API ILoggerOutput {
    public:
        virtual void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
        virtual void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection);
    };

    class TOLCAT_API LoggerFileOutput : public ILoggerOutput {
    private:
        std::ofstream logFileStream;
    public:
        explicit LoggerFileOutput(const std::filesystem::path &logsDir);
        void logInfo(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logWarn(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
        void logError(const std::string &timestamp, const std::string &nameSection, const std::string &messageSection) override;
    };

    class TOLCAT_API LoggerConsoleOutput : public ILoggerOutput {
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
        static std::vector<ILoggerOutput> loggerOutputs;

    public:
        template<class TLoggerOutput>
        inline static void addLoggerOutput(TLoggerOutput loggerOutput) {
            static_assert(std::is_base_of_v<ILoggerOutput, TLoggerOutput>, "Class of TLoggerOutput must derive from ILoggerOutput");
            loggerOutputs.push_back(loggerOutput);
        }
    };
}

#endif // TOLCAT_LOGGER_H_