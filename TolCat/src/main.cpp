#include <filesystem>
#include <memory>

#include <windows.h>

#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"
#include "gluon_logger.hpp"

#include "Gluon/include/gluon_logging.hpp"
#include "Gluon/include/tracers.hpp"
#include "Gluon/include/il2cpp_functions.hpp"

#include "Dobby/include/dobby.h"

TolCat::Logger tolCatLogger_("TolCat");

static struct MainModule {
    void (*loadFunction)();
} mainModule;

int (*initOrig)(const char *);
int tolCatInitialise(const char *domainName) {
    tolCatLogger_.info("IL2CPP initialised in domain: {}", domainName);

    const int retVal = initOrig(domainName);

    //int version = (*Gluon::Il2CppFunctions::globalMetadataHeaderPtr)->version;
    //tolCatLogger_.info("IL2CPP global metadata header version: {}", version);

#ifdef MOD_ENTRY_POINT
    loadMod(&mainModule);

    if (mainModule.loadFunction) {
        mainModule.loadFunction();
    }
#endif

    return retVal;
}

extern "C" [[maybe_unused]] TOLCAT_API void launchTolCat(TolCatLaunchArgs launchArgs) {
    TolCat::Files::createLogsDir();

    // Add the log file output
    TolCat::Logger::addLoggerOutput(
            std::make_unique<TolCat::LoggerFileOutput>(TolCat::Files::getLogsDir())
            );

    // TODO: Console launched output
    // Add the debug console output
    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::kDebugConsole)) {
        TolCat::Logger::addLoggerOutput(std::make_unique<TolCat::LoggerConsoleOutput>());
    }

    TolCat::Logger::beginLog(); // TODO: End log at some point

    tolCatLogger_.info("Logger initialised.");

    tolCatLogger_.info("Initialising Gluon...");
    Gluon::Logger::init(
            std::make_unique<TolCat::GluonLogger>()
            );
    //Gluon::XrefHelpers::initialiseCapstone();
    Gluon::Il2CppFunctions::initialise();
    tolCatLogger_.info("Finished Gluon initialisation!");

    // TODO: Check hook was successful
    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_init),
                    reinterpret_cast<void *>(tolCatInitialise), reinterpret_cast<void **>(&initOrig));
}