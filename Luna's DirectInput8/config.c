#include "pch.h"
#include "config.h"
#include <shlwapi.h>
#include <ShlObj.h>
#include <KnownFolders.h>

/**
 * Returns the path of the config file, creating the containing folders if they don't already exist.
 */
static LPWSTR getConfigPath(void) {
    static WCHAR configPath[MAX_PATH] = L"";
    if (configPath[0] == '\0') {
        PWSTR appDataFolder;
        WCHAR configDirectory[MAX_PATH] = L"";
        SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &appDataFolder);
        PathCombineW(configDirectory, appDataFolder, L"Luna-Project64\\Config"); //Creates config folder, required for PJ64 1.6 (only when loading?)
        CoTaskMemFree(appDataFolder);
        CreateDirectoryW(configDirectory, NULL);
        PathCombineW(configPath, configDirectory, L"Lunaconfig.bin");
    }
    return configPath;
}

void saveConfig(void) {
    FILE* cptr;
    errno_t cfgerr = _wfopen_s(&cptr, getConfigPath(), L"wb"); //Creates or opens config file

    if (cptr != 0) {
        fwrite(&config, sizeof(config), 1, cptr);
        fflush(cptr);
        fclose(cptr);
    }
}

void loadConfig(void) {
    FILE* cptr;
    errno_t cfgerr = _wfopen_s(&cptr, getConfigPath(), L"rb"); //Opens config file to read

    if (cfgerr) {
        restoreDefaults();
    }
    else {
        fread(&config, sizeof(config), 1, cptr);
        fclose(cptr);
    }

    if (config.configVersion != 0x0100) {
        restoreDefaults();
    }
}

void restoreDefaults(void) {
    config = defaultConfig;
}