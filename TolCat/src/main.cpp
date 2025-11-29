#include <filesystem>
#include <memory>

#include <windows.h>

#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"

#include "Gluon/include/gluon_logging.hpp"
#include "Gluon/include/tracers.hpp"
#include "Gluon/include/il2cpp_functions.hpp"

#include "Dobby/include/dobby.h"

#include "spdlog/spdlog.h"

static struct MainModule {
    void (*loadFunction)();
} mainModule;

int (*initOrig)(const char *);
int tolCatInitialise(const char *domainName) {
    TolCat::getLogger()->info("IL2CPP initialised in domain: {}", domainName);

    const int retVal = initOrig(domainName);

    // version = (*Gluon::Il2CppFunctions::globalMetadataHeaderPtr)->version;
    //tolCatLogger_.info("IL2CPP global metadata header version: {}", version);

#ifdef MOD_ENTRY_POINT
    loadMod(&mainModule);

    if (mainModule.loadFunction) {
        mainModule.loadFunction();
    }
#endif

    return retVal;
}

void initLogEnvironment(const TolCatLaunchArgs launchArgs) {
    TolCat::createLogsDir();

    TolCat::archiveLog(TolCat::getLogsDir());
    TolCat::deleteOldLogs(TolCat::getLogsDir());

    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::kDebugConsole)) {
        TolCat::createConsole();
        TolCat::bindConsole();
    }
}

void initLogger(std::shared_ptr<spdlog::logger> logger, const TolCatLaunchArgs launchArgs) {
    const auto sinkMaker = TolCat::SinkMaker(TolCat::getLogsDir());
    TolCat::getLogger()->sinks().push_back(std::move(sinkMaker.createFileSink()));

    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::kDebugConsole)) {
        TolCat::getLogger()->sinks().push_back(std::move(sinkMaker.createConsoleSink()));
    }
}

extern "C" [[maybe_unused]] TOLCAT_API void launchTolCat(TolCatLaunchArgs launchArgs) {
    initLogEnvironment(launchArgs);
    spdlog::info("Initalised log environment.");

    initLogger(TolCat::getLogger(), launchArgs);
    TolCat::getLogger()->info("TolCat logger initialised.");

    TolCat::getLogger()->info("Initialising Gluon...");

    Gluon::Il2CppFunctions::initialise();
    TolCat::getLogger()->info("Finished Gluon initialisation!");

    // TODO: Check hook was successful
    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_init),
                    reinterpret_cast<void *>(tolCatInitialise), reinterpret_cast<void **>(&initOrig));
}