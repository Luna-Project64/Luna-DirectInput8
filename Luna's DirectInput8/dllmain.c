// dllmain.c : Defines the entry point for the DLL application.
#include "pch.h"

#define PLUGIN_NAME "Luna's DirectInput8"
#define PLUGIN_VERSION "1.0.4"
#define PLUGIN_NAMEVER PLUGIN_NAME " v" PLUGIN_VERSION
#define PLUGIN_REPO "https://github.com/LunaticShiN3/Luna-DirectInput8"

#include "zilmar_controller_1.0.h"
#include "directinput.h"
#include "config.h"
#include "gui.h"

HMODULE hModuleVariable;
HWND hMainWindowVariable;

static DInput* gKeyboard = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    hModuleVariable = hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

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

/*EXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam) {
    //stuff here later
}

EXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam) {
    //stuff here later
}*/