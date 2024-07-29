#include <memory>

#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"
#include "gluon_logger.hpp"

#include "Gluon/include/backtrace_helpers.hpp"
#include "Gluon/include/gluon_logging.hpp"
#include "Gluon/include/xref_helpers.hpp"
#include "Gluon/include/il2cpp_functions.hpp"
#include "Gluon/include/exceptions.hpp"

#include "Dobby/include/dobby.h"

TolCat::Logger tolCatLogger_("TolCat");

int (*initOrig)(const char *);
int tolCatInitialise(const char *domainName) {
    tolCatLogger_.info("IL2CPP initialised in domain: {}", domainName);

    int retVal = initOrig(domainName);

    int version = (*Gluon::Il2CppFunctions::globalMetadataHeaderPtr)->version;
    tolCatLogger_.info("IL2CPP global metadata header version: {}", version);

    return retVal;
}

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

    tolCatLogger_.info("Initialising Gluon...");
    Gluon::Logging::Logger::setLoggerAdapter(
            std::make_unique<TolCat::GluonLogger>()
            );
    Gluon::XrefHelpers::initialiseCapstone();
    Gluon::Il2CppFunctions::initialise();
    tolCatLogger_.info("Finished Gluon initialisation!");

    tolCatLogger_.info("Class::Init Address: {:p}", reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_Class_Init));
    auto fn = reinterpret_cast<uint8_t *>(Gluon::Il2CppFunctions::il2cpp_Class_Init);
    tolCatLogger_.info("Class::Init First Bytes: {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x} {:x}", fn[0], fn[1], fn[2], fn[3], fn[4], fn[5], fn[6], fn[7], fn[8], fn[9], fn[10], fn[11]);

    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_init),
                    reinterpret_cast<void *>(tolCatInitialise), reinterpret_cast<void **>(&initOrig));
}