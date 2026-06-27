#include "config.h"
#include <shlwapi.h>
#include <ShlObj.h>
#include <KnownFolders.h>

extern char gPluginConfigDir[MAX_PATH];

/**
 * Returns the path of the config file, creating the containing folders if they don't already exist.
 */
static const char* getConfigPath(void) {
    static char configPath[MAX_PATH];
    if (*gPluginConfigDir)
    {
        strcpy_s(configPath, MAX_PATH, gPluginConfigDir);
    }
    else
    {
        SHGetFolderPath(NULL,
            CSIDL_APPDATA,
            NULL,
            0,
            configPath);
    }

    PathAppendA(configPath, "Luna-Project64"); //Creates config folder, required for PJ64 1.6 (only when loading?)
    CreateDirectoryA(configPath, NULL);
    PathAppendA(configPath, "Config"); //Creates config folder, required for PJ64 1.6 (only when loading?)
    CreateDirectoryA(configPath, NULL);
    PathAppendA(configPath, "Lunaconfig.bin");

	return configPath;
}

void saveConfig(void) {
    FILE* cptr;
    errno_t cfgerr = fopen_s(&cptr, getConfigPath(), "wb"); //Creates or opens config file
    if (cfgerr)
        cptr = NULL;

    if (cptr != 0) {
        fwrite(&config, sizeof(config), 1, cptr);
        fflush(cptr);
        fclose(cptr);
    }
}

void loadConfig(void) {
    FILE* cptr;
    errno_t cfgerr = fopen_s(&cptr, getConfigPath(), "rb"); //Opens config file to read

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