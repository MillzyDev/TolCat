cmake_minimum_required(VERSION 3.27)
project(TolCat)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # better intellisense

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_compile_options(-fdeclspec) # dllexport
add_compile_options(-O3)

set(NO_MOD_MODE ON)

if (WIN32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Gluon")

macro (RECURSE_FILES return_list filter)
    file(GLOB_RECURSE new_list ${filter})
    set(file_list "")
    foreach (file_path ${new_list})
        set(file_list ${file_list} ${file_path})
    endforeach ()
    list(REMOVE_DUPLICATES file_list)
    set(${return_list} ${file_list})
endmacro ()

# Unity Installation
include(unity.cmake)
add_compile_definitions(
        UNITY_MAJOR=\"${UNITY_MAJOR}\"
        UNITY_MINOR=\"${UNITY_MINOR}\"
        UNITY_PATCH=\"${UNITY_PATCH}\"

        UNITY_${UNITY_MAJOR}
        UNITY_${UNITY_MAJOR}_${UNITY_MINOR}
        UNITY_${UNITY_MAJOR}_${UNITY_MINOR}_${UNITY_PATCH}
)

# IL2CPP
set(IL2CPP_DIR "${UNITY_DIRECTORY}/Editor/Data/il2cpp/libil2cpp")
if (WIN32)
    add_compile_definitions(
            IL2CPP_TARGET_CUSTOM=1
            IL2CPP_TARGET_WINDOWS=1
            IL2CPP_TARGET_WINDOWS_DESKTOP=1

            IL2CPP_PLATFORM_SUPPORTS_SYSTEM_CERTIFICATES=1
            IL2CPP_PLATFORM_SUPPORTS_CPU_INFO=1

            _UNICODE=1
            UNICODE=1
            STRICT=1
    )
endif ()
include_directories(${IL2CPP_DIR})

# Mod Properties
include(mod.cmake)

if (${NO_MOD_MODE})
    set(MOD_NAME "TolCat")
    add_compile_definitions(
        MOD_NAME=\"${MOD_NAME}\"
        MOD_VERSION=\"0.0.0\"
    )
else ()
    add_compile_definitions(
        MOD_NAME=\"${MOD_NAME}\"
        MOD_VERSION=\"${MOD_VERSION}\"
        MOD_ENTRY_POINT=\"${MOD_ENTRY_POINT}\"
    )
endif ()

# Game Properties
add_compile_definitions(
        GAME_COMPANY=\"${GAME_COMPANY}\"
        GAME_NAME=\"${GAME_NAME}\"
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Dobby)

# TolCat Target
add_library(TolCat SHARED)

set_target_properties(TolCat PROPERTIES OUTPUT_NAME "TolCat")
set_target_properties(TolCat PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

target_include_directories(
        TolCat PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/TolCat/include"
        TolCat PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/"
        TolCat PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Dobby/include"
)

RECURSE_FILES(tolcat_sources "${CMAKE_CURRENT_SOURCE_DIR}/TolCat/src/*.cpp")
target_sources(
        TolCat
        PRIVATE
        ${tolcat_sources}
)

target_compile_definitions(TolCat PRIVATE TOLCAT_EXPORT) # should only defined for this target

target_link_libraries(TolCat PRIVATE Gluon)
target_link_libraries(TolCat PRIVATE dobby)

# Preloader
add_library(Preloader SHARED)

set_target_properties(Preloader PROPERTIES OUTPUT_NAME "Preloader")
set_target_properties(Preloader PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if (UNITY_MAJOR_AGNOSTIC)
    target_compile_definitions(Preloader PRIVATE UNITY_MAJOR_AGNOSTIC)
endif ()
if (UNITY_MINOR_AGNOSTIC)
    target_compile_definitions(Preloader PRIVATE UNITY_MINOR_AGNOSTIC)
endif ()
if (UNITY_PATCH_AGNOSTIC)
    target_compile_definitions(Preloader PRIVATE UNITY_PATCH_AGNOSTIC)
endif ()

if (GAME_AGNOSTIC)
    target_compile_definitions(Preloader PRIVATE GAME_AGNOSTIC)
endif ()

target_include_directories(
        Preloader PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Preloader/include"
)

RECURSE_FILES(preloader_sources "${CMAKE_CURRENT_SOURCE_DIR}/Preloader/src/*.cpp")
target_sources(
        Preloader
        PRIVATE
        ${preloader_sources}
)

# version.dll Proxy
if (WIN32)
    add_library(Proxy SHARED)

    set_target_properties(Proxy PROPERTIES OUTPUT_NAME "version")
    set_target_properties(Proxy PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

    RECURSE_FILES(proxy_sources "${CMAKE_CURRENT_SOURCE_DIR}/Proxy/*.cpp")
    target_sources(
            Proxy
            PRIVATE
            ${proxy_sources}
            "${CMAKE_CURRENT_SOURCE_DIR}/Proxy/version.def"
    )

    target_link_options(Proxy PRIVATE "-static")
endif ()

file(READ game_path.txt GAME_PATH)

message(STATUS ${MOD_NAME})

if (WIN32)
    add_custom_command(
        TARGET TolCat POST_BUILD
        COMMAND powershell.exe
                -File ${CMAKE_CURRENT_SOURCE_DIR}/copy-to-target.ps1
                -modName "${MOD_NAME}"
                -targetGameDir "${GAME_PATH}"
                -buildDir "${CMAKE_CURRENT_BINARY_DIR}"
    )
    add_custom_command(
            TARGET Preloader POST_BUILD
            COMMAND powershell.exe
            -File ${CMAKE_CURRENT_SOURCE_DIR}/copy-to-target.ps1
            -modName "${MOD_NAME}"
            -targetGameDir "${GAME_PATH}"
            -buildDir "${CMAKE_CURRENT_BINARY_DIR}"
    )
    add_custom_command(
            TARGET Proxy POST_BUILD
            COMMAND powershell.exe
            -File ${CMAKE_CURRENT_SOURCE_DIR}/copy-to-target.ps1
            -modName "${MOD_NAME}"
            -targetGameDir "${GAME_PATH}"
            -buildDir "${CMAKE_CURRENT_BINARY_DIR}"
    )
else ()
# TODO
endif ()