#ifndef _DIRECTINPUT_H
#define _DIRECTINPUT_H

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

typedef struct _DInput* DInput;
struct DInputState
{
	BYTE deviceState[256];
};

DInput* DInputInit(HINSTANCE hinst, HWND hwnd);
void DInputDeinit(DInput*);
struct DInputState DInputGetKeys(DInput*, HINSTANCE hinst, HWND hwnd);
wchar_t* DInputGetKeyName(DInput*, BYTE returnVariable);

void DInputCloseDll(void);

#endif