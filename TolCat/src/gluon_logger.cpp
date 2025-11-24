#include "gluon_logger.hpp"

#include "logger.hpp"

namespace TolCat {
    TolCat::Logger GluonLogger::gluonLogger("Gluon");

    void GluonLogger::log(const std::string &message) {
        TolCat::Logger::neutral("{}", message);
    }

    void GluonLogger::logInfo(const std::string &message) {
        gluonLogger.info("{}", message);
    }

    void GluonLogger::logWarning(const std::string &message) {
        gluonLogger.warn("{}", message);
    }

    void GluonLogger::logError(const std::string &message) {
        gluonLogger.error("{}", message);
    }

    void GluonLogger::logDebug(const std::string &message) {
        gluonLogger.debug("{}", message);
    }
}