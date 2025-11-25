#include "gluon_logger.hpp"

#include "logger.hpp"

namespace TolCat {
    TolCat::Logger GluonLogger::gluonLogger("Gluon");

    void GluonLogger::log(std::string_view fmt, std::format_args args) {
        gluonLogger.neutralFormat(fmt, args);
    }

    void GluonLogger::logInfo(std::string_view fmt, std::format_args args) {
        gluonLogger.infoFormat(fmt, args);
    }

    void GluonLogger::logWarning(std::string_view fmt, std::format_args args) {
        gluonLogger.warnFormat(fmt, args);
    }

    void GluonLogger::logError(std::string_view fmt, std::format_args args) {
        gluonLogger.errorFormat(fmt, args);
    }

    void GluonLogger::logDebug(std::string_view fmt, std::format_args args) {
        gluonLogger.debugFormat(fmt, args);
    }
} // TolCat