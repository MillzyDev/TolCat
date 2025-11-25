#ifndef TOLCAT_GLUON_LOGGER_HPP_
#define TOLCAT_GLUON_LOGGER_HPP_

#include "logger.hpp"
#include "Gluon/include/gluon_logging.hpp"

namespace TolCat {
    class GluonLogger : public Gluon::LoggerAdapter {
    private:
        static TolCat::Logger gluonLogger;
    public:
        void log(std::string_view fmt, std::format_args args) override;
        void logInfo(std::string_view fmt, std::format_args args) override;
        void logWarning(std::string_view fmt, std::format_args argse) override;
        void logError(std::string_view fmt, std::format_args args) override;
        void logDebug(std::string_view fmt, std::format_args args) override;
    };
}

#endif