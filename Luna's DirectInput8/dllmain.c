#define PLUGIN_NAME "Luna's DirectInput8"
#define PLUGIN_VERSION "2.1"
#define PLUGIN_NAMEVER PLUGIN_NAME " v" PLUGIN_VERSION
#define PLUGIN_REPO "https://github.com/Luna-Project64/Luna-DirectInput8"

#include "zilmar_controller_1.0.h"
#include "directinput.h"
#include "config.h"
#include "gui.h"

#include <stdint.h>

HWND hMainWindowVariable;

extern HINSTANCE DINPUT_instance;
#define hModuleVariable DINPUT_instance

static DInput* gKeyboard = NULL;

EXPORT void CALL CloseDLL(void) {
    if (gKeyboard)
    {
        DInputDeinit(gKeyboard);
    }

    DInputCloseDll();
}

EXPORT void CALL DllAbout(HWND hParent) {
    MessageBoxA(
        hParent,
        "DirectInput8 plugin designed as a Jabo alternative for keyboard\n"
        "Made by ShiN3\n"
        "Version " PLUGIN_VERSION "\n"
        "Compiled on " __DATE__ "\n\n"
        PLUGIN_REPO,
        "About " PLUGIN_NAMEVER,
        MB_OK | MB_ICONINFORMATION
    );
}

EXPORT void CALL DllConfig(HWND hParent) {
    loadConfig();
    OpenDialog(hModuleVariable, hParent);
}

EXPORT void CALL GetDllInfo(PLUGIN_INFO* PluginInfo) {
    PluginInfo->Version = 0x0100;
    PluginInfo->Type = PLUGIN_TYPE_CONTROLLER;
    strncpy_s(
        PluginInfo->Name,
        sizeof(PluginInfo->Name),
        PLUGIN_NAMEVER,
        sizeof(PluginInfo->Name)
    );
}

EXPORT void CALL GetKeys(int Control, BUTTONS* Keys) {
    if (!gKeyboard)
        return;

    struct DInputState state = DInputGetKeys(gKeyboard, hModuleVariable, hMainWindowVariable);
    BYTE* deviceState = state.deviceState;

    BYTE integerX;
    BYTE integerY;

    float floatX;
    float floatY;

    Keys->R_DPAD = (deviceState[config.keybindDpadRight] >> 7);
    Keys->L_DPAD = (deviceState[config.keybindDpadLeft] >> 7);
    Keys->D_DPAD = (deviceState[config.keybindDpadDown] >> 7);
    Keys->U_DPAD = (deviceState[config.keybindDpadUp] >> 7);
    Keys->START_BUTTON = (deviceState[config.keybindStart] >> 7);
    Keys->Z_TRIG = (deviceState[config.keybindZ] >> 7);
    Keys->B_BUTTON = (deviceState[config.keybindB] >> 7);
    Keys->A_BUTTON = (deviceState[config.keybindA] >> 7);
    Keys->R_CBUTTON = (deviceState[config.keybindCRight] >> 7);
    Keys->L_CBUTTON = (deviceState[config.keybindCLeft] >> 7);
    Keys->D_CBUTTON = (deviceState[config.keybindCDown] >> 7);
    Keys->U_CBUTTON = (deviceState[config.keybindCUp] >> 7);
    Keys->R_TRIG = (deviceState[config.keybindR] >> 7);
    Keys->L_TRIG = (deviceState[config.keybindL] >> 7);

    if (deviceState[config.keybindLeft] >> 7) {
        integerX = -1;
    }
    else if (deviceState[config.keybindRight] >> 7) {
        integerX = 1;
    }
    else {
        integerX = 0;
    }
    if (deviceState[config.keybindDown] >> 7) {
        integerY = -1;
    }
    else if (deviceState[config.keybindUp] >> 7) {
        integerY = 1;
    }
    else {
        integerY = 0;
    }

    if (integerX != 0 && integerY != 0) {
        Keys->Y_AXIS = integerX * config.rangeDiagonalX;
        Keys->X_AXIS = integerY * config.rangeDiagonalY;
    }
    else {
        Keys->Y_AXIS = integerX * config.rangeCardinalX;
        Keys->X_AXIS = integerY * config.rangeCardinalY;
    }

    for (int i = 0; i < sizeof(config.modifiers) / sizeof(Modifier); i++) {
        if ((config.modifiers[i].keybind != 0) && (deviceState[config.modifiers[i].keybind] >> 7)) {
            Keys->Y_AXIS = (float)Keys->Y_AXIS * config.modifiers[i].multiplierX; //X and Y axis are swapped because of course they are????
            Keys->X_AXIS = (float)Keys->X_AXIS * config.modifiers[i].multiplierY;
        }
    }
}

EXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4])
{
    for (int i = 0; i < 4; ++i)
    {
        Controls[i].Present = FALSE; //Enables controller 1 only. Might change later.
        Controls[i].RawData = FALSE;
        Controls[i].Plugin = 2;
    }
    Controls[0].Present = TRUE;

    hMainWindowVariable = hMainWindow;
}

EXPORT void CALL RomClosed(void)
{
    if (!gKeyboard)
        return;

    DInputDeinit(gKeyboard);
    gKeyboard = NULL;
}

EXPORT void CALL RomOpen(void) {
    loadConfig();
    gKeyboard = DInputInit(hModuleVariable, hMainWindowVariable);
}

char gPluginConfigDir[MAX_PATH] = {0};
short Set_PluginConfigDir = 0;
enum SettingLocation
{
    SettingType_ConstString = 0,
    SettingType_ConstValue = 1,
    SettingType_CfgFile = 2,
    SettingType_Registry = 3,
    SettingType_RelativePath = 4,
    TemporarySetting = 5,
    SettingType_RomDatabase = 6,
    SettingType_CheatSetting = 7,
    SettingType_GameSetting = 8,
    SettingType_BoolVariable = 9,
    SettingType_NumberVariable = 10,
    SettingType_StringVariable = 11,
    SettingType_SelectedDirectory = 12,
    SettingType_RdbSetting = 13,
};

enum SettingDataType
{
    Data_DWORD = 0, Data_String = 1, Data_CPUTYPE = 2, Data_SelfMod = 3, Data_OnOff = 4, Data_YesNo = 5, Data_SaveChip = 6
};

typedef struct
{
    uint32_t dwSize;
    int DefaultStartRange;
    int SettingStartRange;
    int MaximumSettings;
    int NoDefault;
    int DefaultLocation;
    void* handle;

    unsigned int(CALL* GetSetting)      (void* handle, int ID);
    const char* (CALL* GetSettingSz)    (void* handle, int ID, char* Buffer, int BufferLen);
    void(CALL* SetSetting)      (void* handle, int ID, unsigned int Value);
    void(CALL* SetSettingSz)    (void* handle, int ID, const char* Value);
    void(CALL* RegisterSetting) (void* handle, int ID, int DefaultID, enum SettingDataType Type,
        enum SettingLocation Location, const char* Category, const char* DefaultStr, uint32_t Value);
    void(CALL* UseUnregisteredSetting) (int ID);
} PLUGIN_SETTINGS;

typedef struct
{
    unsigned int(CALL* FindSystemSettingId) (void* handle, const char* Name);
} PLUGIN_SETTINGS2;

static PLUGIN_SETTINGS  g_PluginSettings;
static PLUGIN_SETTINGS2 g_PluginSettings2;
static inline unsigned int GetSystemSetting(short SettingID)
{
    return g_PluginSettings.GetSetting(g_PluginSettings.handle, SettingID);
}

static inline short FindSystemSettingId(const char* Name)
{
    if (g_PluginSettings2.FindSystemSettingId && g_PluginSettings.handle)
    {
        return (short)g_PluginSettings2.FindSystemSettingId(g_PluginSettings.handle, Name);
    }
    return 0;
}

EXPORT void CALL SetSettingInfo(PLUGIN_SETTINGS* info)
{
    g_PluginSettings = *info;
}

EXPORT void CALL SetSettingInfo2(PLUGIN_SETTINGS2* info)
{
    g_PluginSettings2 = *info;
}

EXPORT void CALL PluginLoaded(void)
{
    int pluginConfigDir = FindSystemSettingId("Config Base Dir");
    if (pluginConfigDir)
    {
        const char* cfg = g_PluginSettings.GetSettingSz(g_PluginSettings.handle, pluginConfigDir, gPluginConfigDir, sizeof(gPluginConfigDir));
        if (!cfg)
            *gPluginConfigDir = '\0';
    }
}

/*EXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam) {
    //stuff here later
}

EXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam) {
    //stuff here later
}*/