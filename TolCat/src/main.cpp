#include <filesystem>
#include <memory>

#include <windows.h>

#include "launch_flags.hpp"
#include "logger.hpp"
#include "files.hpp"
#include "gluon_logger.hpp"

#include "Gluon/include/gluon_logging.hpp"
#include "Gluon/include/xref_helpers.hpp"
#include "Gluon/include/il2cpp_functions.hpp"

#include "Dobby/include/dobby.h"

#include "GlobalNamespace/ColorChanger.hpp"
#include "UnityEngine/Color.hpp"

TolCat::Logger tolCatLogger_("TolCat");

static struct MainModule {
    void (*loadFunction)();
} mainModule;

void loadMod(MainModule *module) {
#ifdef WIN32
    constexpr auto kSharedExtension = ".dll";
#elif __linux__
    constexpr auto kSharedExtension = ".so";
// Linux supported in the future at some point
#else
#error "Unsupported Platform"
#endif

    tolCatLogger_.info("--------------------------------------------------");
    tolCatLogger_.info("|#############   MAIN MOD LOADING   #############|");
    tolCatLogger_.info("--------------------------------------------------");
    tolCatLogger_.info("Loading main module: {} v{}", MOD_NAME, MOD_VERSION);

    std::filesystem::path modPath = TolCat::Files::getModDir() / (std::string(MOD_NAME) + kSharedExtension);
    tolCatLogger_.info("Attempting load of: {}", modPath);
    HMODULE modHandle = LoadLibraryExA(modPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!modHandle) {
        tolCatLogger_.error("Failed to load main module.");
        // TODO: Error somehow
        return;
    }

    tolCatLogger_.info("Calling into main module...");
    module->loadFunction() = GetProcAddress(modHandle, "load");
    tolCatLogger_.info("Main module fully loaded!");

    tolCatLogger_.info("--------------------------------------------------");
    tolCatLogger_.info("|#############   MAIN MOD LOADED!   #############|");
    tolCatLogger_.info("--------------------------------------------------");
}

int (*initOrig)(const char *);
int tolCatInitialise(const char *domainName) {
    tolCatLogger_.info("IL2CPP initialised in domain: {}", domainName);

    const int retVal = initOrig(domainName);

    int version = (*Gluon::Il2CppFunctions::globalMetadataHeaderPtr)->version;
    tolCatLogger_.info("IL2CPP global metadata header version: {}", version);

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

    GlobalNamespace::ColorChanger *colorChanger = nullptr; // just for demo purposes
    UnityEngine::Material *material = colorChanger->_material;
    UnityEngine::Color color = material->color;

    (void)DobbyHook(reinterpret_cast<void *>(Gluon::Il2CppFunctions::il2cpp_init),
                    reinterpret_cast<void *>(tolCatInitialise), reinterpret_cast<void **>(&initOrig));
}