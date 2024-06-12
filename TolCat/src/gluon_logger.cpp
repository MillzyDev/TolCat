#include "gluon_logger.hpp"

#include "logger.hpp"

namespace TolCat {
    TolCat::Logger GluonLogger::gluonLogger("Gluon");

    void GluonLogger::logNeutral(const std::string &message) {
        TolCat::Logger::neutral("{}", message);
    }

    void GluonLogger::logInfo(const std::string &message) {
        gluonLogger.info("{}", message);
    }

    void GluonLogger::logWarn(const std::string &message) {
        gluonLogger.warn("{}", message);
    }

    void GluonLogger::logError(const std::string &message) {
        gluonLogger.error("{}", message);
    }
}