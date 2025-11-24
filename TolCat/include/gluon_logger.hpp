#ifndef TOLCAT_GLUON_LOGGER_HPP_
#define TOLCAT_GLUON_LOGGER_HPP_

#include "logger.hpp"
#include "Gluon/include/gluon_logging.hpp"

namespace TolCat {
    class GluonLogger : public Gluon::LoggerAdapter {
    private:
        static TolCat::Logger gluonLogger;
    public:
        void log(const std::string &message) override;
        void logInfo(const std::string &message) override;
        void logWarning(const std::string &message) override;
        void logError(const std::string &message) override;
        void logDebug(const std::string &message) override;
    };
}

#endif