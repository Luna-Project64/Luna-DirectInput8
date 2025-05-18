#include "directinput.h"

typedef struct Modifier {
    byte keybind;
    float multiplierX;
    float multiplierY;
} Modifier;

typedef struct Config {
    int configVersion;

    byte keybindDpadRight;
    byte keybindDpadLeft;
    byte keybindDpadDown;
    byte keybindDpadUp;
    byte keybindStart;
    byte keybindZ;
    byte keybindB;
    byte keybindA;
    byte keybindCRight;
    byte keybindCLeft;
    byte keybindCDown;
    byte keybindCUp;
    byte keybindR;
    byte keybindL;

    byte keybindRight;
    byte keybindLeft;
    byte keybindDown;
    byte keybindUp;

    byte rangeCardinalX;
    byte rangeCardinalY;
    byte rangeDiagonalX;
    byte rangeDiagonalY;

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
