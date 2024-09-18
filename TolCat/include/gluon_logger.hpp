#ifndef TOLCAT_GLUON_LOGGER_HPP_
#define TOLCAT_GLUON_LOGGER_HPP_

#include "logger.hpp"
#include "Gluon/include/gluon_logging.hpp"

namespace TolCat {
    class GluonLogger : public Gluon::Logging::LoggerAdapter {
    private:
        static TolCat::Logger gluonLogger;
    public:
        void logNeutral(const std::string &message) override;
        void logInfo(const std::string &message) override;
        void logWarn(const std::string &message) override;
        void logError(const std::string &message) override;
        void logDebug(const std::string &message) override;
    };
}

#endif