#include <memory>

#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"

TolCat::Logger tolCatLogger_("TolCat");

extern "C" [[maybe_unused]] __declspec(dllexport) void launchTolCat(TolCatLaunchArgs launchArgs) {
    TolCat::Files::createLogsDir();

    // Add the log file output
    TolCat::Logger::addLoggerOutput(
            std::make_unique<TolCat::LoggerFileOutput>(TolCat::Files::getLogsDir())
            );

    // Add the debug console output
    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::kDebugConsole)) {
        TolCat::Logger::addLoggerOutput(std::make_unique<TolCat::LoggerConsoleOutput>());
    }

    tolCatLogger_.info("Logger initialised.");
}