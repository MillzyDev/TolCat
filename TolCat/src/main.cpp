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

void (*shutdownOrig)();
void tolCatShutdown() {
    TolCat::getLogger()->info("IL2CPP shutting down.");
    TolCat::getLogger()->info("Shutting down loggers.");
    TolCat::getLogger()->error("SHUTDOWN");
    spdlog::shutdown();
    std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));

    shutdownOrig();
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
    static const auto sinkMaker = TolCat::SinkMaker(TolCat::getLogsDir());
    logger->sinks().push_back(std::move(sinkMaker.createFileSink()));

    if (hasLaunchArg(launchArgs, TolCatLaunchArgs::kDebugConsole)) {
        logger->sinks().push_back(std::move(sinkMaker.createConsoleSink()));
    }

    logger->flush_on(spdlog::level::err);
}

extern "C" [[maybe_unused]] TOLCAT_API void launchTolCat(TolCatLaunchArgs launchArgs) {
    initLogEnvironment(launchArgs);
    spdlog::info("Initialised log environment.");

    initLogger(TolCat::getLogger(), launchArgs);
    TolCat::getLogger()->info("TolCat logger initialised.");

    TolCat::getLogger()->info("Initialising Gluon...");
    initLogger(Gluon::getLogger(), launchArgs);
    Gluon::Il2CppFunctions::initialise();
    TolCat::getLogger()->info("Finished Gluon initialisation!");

    // TODO: Check hook was successful
    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_init),
                    reinterpret_cast<void *>(tolCatInitialise), reinterpret_cast<void **>(&initOrig));

    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_shutdown),
                    reinterpret_cast<void *>(tolCatShutdown), reinterpret_cast<void **>(&shutdownOrig));
}