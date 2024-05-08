#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"

extern "C" [[maybe_unused]] __declspec(dllexport) void launchTolCat(TolCatLaunchArgs launchArgs) {
    TolCat::Files::createLogsDir();

    // Add the log file output
    TolCat::Logger::addLoggerOutput(
            TolCat::LoggerFileOutput(
                    TolCat::Files::getLogsDir()
                    )
            );

    // Add the debug console output
    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::DEBUG_CONSOLE)) {
        TolCat::Logger::addLoggerOutput(
                TolCat::LoggerConsoleOutput()
                );
    }
}