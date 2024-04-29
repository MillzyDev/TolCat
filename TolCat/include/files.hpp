#pragma once

#include <filesystem>

#include "tolcat_config.hpp"

namespace TolCat::Files {
    /**
     * Constructs and gets the path to the target game's installation directory.
     * @return The path to the target game's installation directory.
     */
    TOLCAT_API std::filesystem::path getBaseDir();

    /**
     * Constructs and gets the path to the directory containing mod files.
     * @return The path to the directory containing mod files.
     */
    TOLCAT_API std::filesystem::path getModDir();

    /**
     * Constructs and gets the path to the addons directory.
     * @return The path to the addons directory.
     */
    TOLCAT_API std::filesystem::path getAddonsDir();

    /**
     * Construct and gets the path to the libraries directory.
     * @return The path to the libraries directory.
     */
    TOLCAT_API std::filesystem::path getUserLibsDir();

    /**
     * Construct and gets the path to the logs directory.
     * @return The path to the logs directory.
     */
    TOLCAT_API std::filesystem::path getLogsDir();
}