#include "pch.h"
#include "directinput.h"
#include "config.h"
#include <shlwapi.h>

static IDirectInput8A* interfacePointer = NULL;

void DInputCloseDll(void)
{
	if (interfacePointer)
		IDirectInputDevice8_Release(interfacePointer);
}

static HRESULT DInputOpenDLL(HINSTANCE hinst)
{
	if (interfacePointer)
		return 0;

	return DirectInput8Create( //Creates a DirectInput8 object.
		hinst, //this has to be hModule
		DIRECTINPUT_VERSION,
		&IID_IDirectInput8A,
		(LPVOID*)&interfacePointer,
		NULL
	);
}

DInput* DInputInit(HINSTANCE hinst, HWND hwnd) {
	char filePath[MAX_PATH];
	GetModuleFileNameA(NULL, filePath, sizeof(filePath));
	PathRemoveFileSpecA(filePath);
	PathCombineA(filePath, filePath, "Logs"); //Creates logs folder, required for PJ64 1.6
	CreateDirectoryA(filePath, NULL);
	PathCombineA(filePath, filePath, "Lunalog.txt"); //Creates or opens log file
	FILE* fptr;
	errno_t err = fopen_s(&fptr, filePath, "w");
	if (err)
		fptr = NULL;

	HRESULT result = DInputOpenDLL(hinst);
	if (fptr != 0) {
		switch (result) {
		case DI_OK:
			fprintf(fptr, "DirectInput8Create DI_OK\n");
			break;
		case DIERR_BETADIRECTINPUTVERSION:
			fprintf(fptr, "DirectInput8Create DIERR_BETADIRECTINPUTVERSION\n");
			break;
		case DIERR_INVALIDPARAM:
			fprintf(fptr, "DirectInput8Create DIERR_INVALIDPARAM\n");
			break;
		case DIERR_OLDDIRECTINPUTVERSION:
			fprintf(fptr, "DirectInput8Create DIERR_OLDDIRECTINPUTVERSION\n");
			break;
		case DIERR_OUTOFMEMORY:
			fprintf(fptr, "DirectInput8Create DIERR_OUTOFMEMORY\n");
			break;
		default:
			fprintf(fptr, "DirectInput8Create %d\n", result);
			break;
		}
	}

	LPDIRECTINPUTDEVICE8A lpdiKeyboard;
	IDirectInput8_CreateDevice( //Creates a keyboard DirectInput device.
		interfacePointer,
		&GUID_SysKeyboard,
		&lpdiKeyboard,
		NULL
	);

	IDirectInputDevice8_SetCooperativeLevel(lpdiKeyboard, hwnd, //Sets cooperative level. Nonexclusive, no background input. Might add settings later. Figure out how to set hwnd https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ee416848(v=vs.85)?
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND); //Add DISCL_NOWINKEY next to exclusivity to disable Windows key.

	result= IDirectInputDevice8_SetDataFormat(lpdiKeyboard, &c_dfDIKeyboard); //Sets data format

	if (fptr != 0) {
		switch (result) {
		case DI_OK:
			fprintf(fptr, "SetDataFormat DI_OK\n");
			break;
		case DIERR_ACQUIRED:
			fprintf(fptr, "SetDataFormat DIERR_ACQUIRED\n");
			break;
		case DIERR_INVALIDPARAM:
			fprintf(fptr, "SetDataFormat DIERR_INVALIDPARAM\n");
			break;
		case DIERR_NOTINITIALIZED:
			fprintf(fptr, "SetDataFormat DIERR_NOTINITIALIZED\n");
			break;
		}
	}

	result = IDirectInputDevice8_Acquire(lpdiKeyboard); //Acquires input device

	if (fptr != 0) {
		switch (result) {
		case DI_OK:
			fprintf(fptr, "Acquire DI_OK\n");
			break;
		case DIERR_INVALIDPARAM:
			fprintf(fptr, "Acquire DIERR_INVALIDPARAM\n");
			break;
		case DIERR_NOTINITIALIZED:
			fprintf(fptr, "Acquire DIERR_NOTINITIALIZED\n");
			break;
		case DIERR_OTHERAPPHASPRIO:
			fprintf(fptr, "Acquire DIERR_OTHERAPPHASPRIO\n");
			break;
		}
		fclose(fptr);
	}

	return lpdiKeyboard;
}

void DInputDeinit(DInput* i)
{
	if (!i)
		return;

	LPDIRECTINPUTDEVICE8A d = (LPDIRECTINPUTDEVICE8A)i;
	IDirectInputDevice8_Unacquire(d);
	IDirectInputDevice8_Release(d);
}

struct DInputState DInputGetKeys(DInput* i, HINSTANCE hinst, HWND hwnd) {
	struct DInputState state = { 0 };
	LPDIRECTINPUTDEVICE8A lpdiKeyboard = (LPDIRECTINPUTDEVICE8A)i;
	if (lpdiKeyboard != NULL) {
		HRESULT result = IDirectInputDevice8_GetDeviceState(lpdiKeyboard, (sizeof(state.deviceState)), (LPVOID*)&state.deviceState);
		state.deviceState[0] = 0;

		if (GetForegroundWindow() == hwnd) {
			if (result == DIERR_INPUTLOST) {
				int i;
				for (i = 0; i < 100; i++) {
					HRESULT result = IDirectInputDevice8_Acquire(lpdiKeyboard);
					if (result != DIERR_OTHERAPPHASPRIO) {
						break;
					}
					Sleep(50);
				}
			}
		}
	}

	return state;
}

DIPROPSTRING DInputGetKeyName(DInput* i, byte returnVariable)
{
	LPDIRECTINPUTDEVICE8A lpdiKeyboard = (LPDIRECTINPUTDEVICE8A)i;
	DIPROPSTRING dips;
	dips.diph.dwSize = sizeof(dips);
	dips.diph.dwHeaderSize = sizeof(dips.diph);
	dips.diph.dwHow = DIPH_BYOFFSET;
	dips.diph.dwObj = returnVariable;

	IDirectInputDevice8_GetProperty(lpdiKeyboard, DIPROP_KEYNAME, &dips.diph); //this bich refuses to be ascii so gotta use unicode functions
	return dips;
}
