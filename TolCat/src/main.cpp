#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"

extern "C" __declspec(dllexport) void launchTolCat(TolCatLaunchArgs launchArgs) {
    TolCat::Logger::addLoggerOutput(
            TolCat::LoggerFileOutput(
                    TolCat::Files::getLogsDir()
                    )
            );
    
    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::DEBUG_CONSOLE)) {
        TolCat::Logger::addLoggerOutput(
                TolCat::LoggerConsoleOutput()
                );
    }
}