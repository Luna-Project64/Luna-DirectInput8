#ifndef _DIRECTINPUT_H
#define _DIRECTINPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include <stdbool.h>
#include <stdio.h>
#include <dinput.h>

typedef struct _DInput* DInput;
struct DInputState
{
	byte deviceState[256];
};

DInput* DInputInit(HINSTANCE hinst, HWND hwnd);
void DInputDeinit(DInput*);
struct DInputState DInputGetKeys(DInput*, HINSTANCE hinst, HWND hwnd);
DIPROPSTRING DInputGetKeyName(DInput*, byte returnVariable);

void DInputCloseDll(void);

#endif