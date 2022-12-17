# It's recommended to set a minimum CMake version.
# If you use CMake features from higher versions, update this to match.
cmake_minimum_required(VERSION 3.21)

# Set your project name. This will be the name of your SKSE .dll file.
project(SkyrimScripting.StartupScript VERSION 0.0.1 LANGUAGES CXX)

# Set <DEPLOY_ROOT> to set the path of a mod to deploy files to.
# Defaults to the value of environment variable SKYRIM_MODS_FOLDER
set(DEPLOY_ROOT "$ENV{SKYRIM_MODS_FOLDER}")
# The SKSE plugin files will be writen to <DEPLOY_ROOT>/<PROJECT_NAME>/SKSE/Plugins
# set(DEPLOY_ROOT "C:/some/path/to/mod/path") # <--- uncomment line to customize

# When enabled (default) your mod's SKSE/Plugins folder will be zipped up using 7-zip
# if 7-zip is located at C:\Program Files\7-Zip\7z.exe or 7ZIP_PATH
set(7ZIP_PATH "C:/Program Files/7-Zip/7z.exe")
option(PACKAGE_MOD "When ON this will create .7z files in your build/mod folder on build" ON)

# Setup your SKSE plugin as an SKSE plugin!
find_package(CommonLibSSE CONFIG REQUIRED)
add_commonlibsse_plugin(${PROJECT_NAME} SOURCES plugin.cpp) # <--- specifies plugin.cpp
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23) # <--- use C++23 standard
target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h) # <--- PCH.h is required!

# When your SKSE .dll is compiled, this will automatically copy the .dll into your mods folder.
# Only works if you configure DEPLOY_ROOT above (or set the SKYRIM_MODS_FOLDER environment variable)
# A .pdb file is also copied when the build configuration is set to Debug
string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
if(EXISTS "${DEPLOY_ROOT}")
    set(MOD_FOLDER "${DEPLOY_ROOT}/${PROJECT_NAME}")
    set(DLL_FOLDER "${MOD_FOLDER}/SKSE/Plugins")
    message("Plugin output directory: ${MOD_FOLDER}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM)
    if(build_type STREQUAL debug)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM)
    endif()
endif()

# Packages your SKSE Plugin into a build/mods/ folder for easy upload
if(PACKAGE_MOD AND EXISTS "${7ZIP_PATH}")
    set(MOD_PACKAGE_FOLDER "${CMAKE_SOURCE_DIR}/build/mods")
    set(MOD_PACKAGE_SKSE_TEMP_FOLDER "${MOD_PACKAGE_FOLDER}/SKSE/Plugins")
    set(MOD_PACKAGE_FILE "${MOD_PACKAGE_FOLDER}/${PROJECT_NAME}-${PROJECT_VERSION}.7z")
    if(build_type STREQUAL debug)
        set(MOD_PACKAGE_FILE "${MOD_PACKAGE_FOLDER}/${PROJECT_NAME}-${PROJECT_VERSION}_Debug.7z")
    endif()
    message("Mod package path: ${MOD_PACKAGE_FILE}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${MOD_PACKAGE_FOLDER}/SKSE"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOD_PACKAGE_SKSE_TEMP_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${MOD_PACKAGE_SKSE_TEMP_FOLDER}/"
        VERBATIM)
    if(build_type STREQUAL debug)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${MOD_PACKAGE_SKSE_TEMP_FOLDER}/"
            VERBATIM)
    endif()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${7ZIP_PATH}" a "${MOD_PACKAGE_FILE}" "${MOD_PACKAGE_SKSE_TEMP_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM)
    if(build_type STREQUAL debug)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${7ZIP_PATH}" a "${MOD_PACKAGE_FILE}" "${MOD_PACKAGE_SKSE_TEMP_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM)
    endif()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E remove_directory "${MOD_PACKAGE_FOLDER}/SKSE"
        VERBATIM)
endif()
