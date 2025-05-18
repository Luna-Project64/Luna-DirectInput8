#pragma once

#include "directinput.h"

typedef struct Modifier {
    BYTE keybind;
    float multiplierX;
    float multiplierY;
} Modifier;

typedef struct Config {
    int configVersion;

    BYTE keybindDpadRight;
    BYTE keybindDpadLeft;
    BYTE keybindDpadDown;
    BYTE keybindDpadUp;
    BYTE keybindStart;
    BYTE keybindZ;
    BYTE keybindB;
    BYTE keybindA;
    BYTE keybindCRight;
    BYTE keybindCLeft;
    BYTE keybindCDown;
    BYTE keybindCUp;
    BYTE keybindR;
    BYTE keybindL;

    BYTE keybindRight;
    BYTE keybindLeft;
    BYTE keybindDown;
    BYTE keybindUp;

    BYTE rangeCardinalX;
    BYTE rangeCardinalY;
    BYTE rangeDiagonalX;
    BYTE rangeDiagonalY;

    Modifier modifiers[50];
} Config;

Config config;

static Config defaultConfig = {
    .configVersion = 0x0100,

    .keybindDpadRight = 0x27, //Ñ
    .keybindDpadLeft = 0x26, //L
    .keybindDpadDown = 0x34, //.
    .keybindDpadUp = 0x19, //P
    .keybindStart = 0x1C, //Enter
    .keybindZ = 0x2C, //Z
    .keybindB = 0x2E, //C
    .keybindA = 0x2D, //X
    .keybindCRight = 0x4D, //Numpad6
    .keybindCLeft = 0x4B, //Numpad4
    .keybindCDown = 0x50, //Numpad2
    .keybindCUp = 0x48, //Numpad8
    .keybindR = 0x1F, //S
    .keybindL = 0x1E, //A

    .keybindRight = 0xCD, //Right arrow key
    .keybindLeft = 0xCB, //Left arrow key
    .keybindDown = 0xD0, //Down arrow key
    .keybindUp = 0xC8, //Up arrow key

    .rangeCardinalX = 126,
    .rangeCardinalY = 126, //These need to be limited to 127 max.
    .rangeDiagonalX = 90,
    .rangeDiagonalY = 89,

    .modifiers = {
        {0x2A, 0.2f , 0.2f},
        {0x39, 1.0f , 0.58f},
        {0x2F, 0.58f, 1.f}
    }
};

void saveConfig(void);
void loadConfig(void);
void restoreDefaults(void);
