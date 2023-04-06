#include "pch.h"
#include "gui.h"
#include "resource.h"
#include "config.h"
#include <windowsx.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <shellapi.h>

HWND hDlgItem;
HWND parentVariable;
int selectedIndex;
int selectedKey;
float selectedX;
float selectedY;

BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    FILE* fptr;
    errno_t err;
    char filePath[MAX_PATH];

    switch (message)
    {
    case WM_INITDIALOG:
        IDirectInputDevice8_Unacquire(lpdiKeyboard);
        HRESULT result = IDirectInputDevice8_SetCooperativeLevel(lpdiKeyboard, hwndDlg, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
        int i;
        for (i = 0; i < 100; i++) {
            HRESULT result = IDirectInputDevice8_Acquire(lpdiKeyboard);
            if (result != DIERR_OTHERAPPHASPRIO) {
                break;
            }
            Sleep(50);
        }

        hDlgItem = GetDlgItem(hwndDlg, IDC_MODIFIERS);
        memset(&LvColumn, 0, sizeof(LvColumn));
        LvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        LvColumn.cx = 0x28;
        LvColumn.pszText = "Key";
        LvColumn.cx = 0x42;
        SendMessageA(hDlgItem, LVM_INSERTCOLUMNA, 0, (LPARAM)&LvColumn);
        LvColumn.pszText = "X";
        SendMessageA(hDlgItem, LVM_INSERTCOLUMNA, 1, (LPARAM)&LvColumn);
        LvColumn.pszText = "Y";
        SendMessageA(hDlgItem, LVM_INSERTCOLUMNA, 2, (LPARAM)&LvColumn);
        LvItem.mask = LVIF_TEXT;
        LvItem.cchTextMax = 256;
        LvItem.pszText = "Init";
        LvItem.iSubItem = 0;
        int i;
        for (i = 0; i < 50; i++) {
            LvItem.iItem = i;
            SendMessageA(hDlgItem, LVM_INSERTITEMA, 0, (LPARAM)&LvItem);
        }
        SendMessageA(hDlgItem, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

        resetButtonLabels(hwndDlg);
        break;

    case WM_CLOSE:
        loadConfig();
        EndDialog(hwndDlg, 0);
        IDirectInputDevice8_Unacquire(lpdiKeyboard);
        IDirectInputDevice8_SetCooperativeLevel(lpdiKeyboard, parentVariable, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
        int i;
        for (i = 0; i < 100; i++) {
            HRESULT result = IDirectInputDevice8_Acquire(lpdiKeyboard);
            if (result != DIERR_OTHERAPPHASPRIO) {
                break;
            }
            Sleep(50);
        }
        break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
            case NM_CLICK:
                hDlgItem = GetDlgItem(hwndDlg, IDC_MODIFIERS);
                selectedIndex = SendMessage(hDlgItem, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
                selectedKey = config.modifiers[selectedIndex].keybind;
                selectedX = config.modifiers[selectedIndex].multiplierX;
                selectedY = config.modifiers[selectedIndex].multiplierY;
                setButtonLabel(hwndDlg, IDC_MODIFIERKEY, config.modifiers[selectedIndex].keybind);
                setFloatEditBoxContent(hwndDlg, IDC_MODIFIERX, &config.modifiers[selectedIndex].multiplierX);
                setFloatEditBoxContent(hwndDlg, IDC_MODIFIERY, &config.modifiers[selectedIndex].multiplierY);
                break;
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == EN_UPDATE) {
            switch (LOWORD(wParam))
            {
                case IDC_CARDINALX:
                    getEditBoxContent(hwndDlg, IDC_CARDINALX, &config.rangeCardinalX);
                    break;
                case IDC_CARDINALY:
                    getEditBoxContent(hwndDlg, IDC_CARDINALY, &config.rangeCardinalY);
                    break;
                case IDC_DIAGONALX:
                    getEditBoxContent(hwndDlg, IDC_DIAGONALX, &config.rangeDiagonalX);
                    break;
                case IDC_DIAGONALY:
                    getEditBoxContent(hwndDlg, IDC_DIAGONALY, &config.rangeDiagonalY);
                    break;

                case IDC_MODIFIERX:
                    getFloatEditBoxContent(hwndDlg, IDC_MODIFIERX, &selectedX);
                    break;
                case IDC_MODIFIERY:
                    getFloatEditBoxContent(hwndDlg, IDC_MODIFIERY, &selectedY);
                    break;
            }
        }
        switch (wParam)
        {
        case IDCANCEL:
            loadConfig();
            EndDialog(hwndDlg, 0);
            IDirectInputDevice8_Unacquire(lpdiKeyboard);
            IDirectInputDevice8_SetCooperativeLevel(lpdiKeyboard, parentVariable, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
            int i;
            for (i = 0; i < 100; i++) {
                HRESULT result = IDirectInputDevice8_Acquire(lpdiKeyboard);
                if (result != DIERR_OTHERAPPHASPRIO) {
                    break;
                }
                Sleep(50);
            }
            break;
        case IDOK:
            saveConfig();
            EndDialog(hwndDlg, 0);
            IDirectInputDevice8_Unacquire(lpdiKeyboard);
            HRESULT result = IDirectInputDevice8_SetCooperativeLevel(lpdiKeyboard, parentVariable, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
            int i;
            for (i = 0; i < 100; i++) {
                HRESULT result = IDirectInputDevice8_Acquire(lpdiKeyboard);
                if (result != DIERR_OTHERAPPHASPRIO) {
                    break;
                }
                Sleep(50);
            }
            break;
        case IDC_RESTOREDEFAULTS:
            restoreDefaults();
            resetButtonLabels(hwndDlg);
            break;

        case IDC_HELP:
            ShellExecuteA(0, 0, "http://sites.google.com/view/shurislibrary/luna-dinput8", 0, 0, SW_HIDE);
            break;


        case IDC_ABUTTON:
            getConfigKey(hwndDlg, IDC_ABUTTON, &config.keybindA);
            break;
        case IDC_BBUTTON:
            getConfigKey(hwndDlg, IDC_BBUTTON, &config.keybindB);
            break;
        case IDC_START:
            getConfigKey(hwndDlg, IDC_START, &config.keybindStart);
            break;
        case IDC_LTRIG:
            getConfigKey(hwndDlg, IDC_LTRIG, &config.keybindL);
            break;
        case IDC_ZTRIG:
            getConfigKey(hwndDlg, IDC_ZTRIG, &config.keybindZ);
            break;
        case IDC_RTRIG:
            getConfigKey(hwndDlg, IDC_RTRIG, &config.keybindR);
            break;

        case IDC_ANALOGLEFT:
            getConfigKey(hwndDlg, IDC_ANALOGLEFT, &config.keybindLeft);
            break;
        case IDC_ANALOGRIGHT:
            getConfigKey(hwndDlg, IDC_ANALOGRIGHT, &config.keybindRight);
            break;
        case IDC_ANALOGUP:
            getConfigKey(hwndDlg, IDC_ANALOGUP, &config.keybindUp);
            break;
        case IDC_ANALOGDOWN:
            getConfigKey(hwndDlg, IDC_ANALOGDOWN, &config.keybindDown);
            break;

        case IDC_CLEFT:
            getConfigKey(hwndDlg, IDC_CLEFT, &config.keybindCLeft);
            break;
        case IDC_CRIGHT:
            getConfigKey(hwndDlg, IDC_CRIGHT, &config.keybindCRight);
            break;
        case IDC_CUP:
            getConfigKey(hwndDlg, IDC_CUP, &config.keybindCUp);
            break;
        case IDC_CDOWN:
            getConfigKey(hwndDlg, IDC_CDOWN, &config.keybindCDown);
            break;

        case IDC_DPADLEFT:
            getConfigKey(hwndDlg, IDC_DPADLEFT, &config.keybindDpadLeft);
            break;
        case IDC_DPADRIGHT:
            getConfigKey(hwndDlg, IDC_DPADRIGHT, &config.keybindDpadRight);
            break;
        case IDC_DPADUP:
            getConfigKey(hwndDlg, IDC_DPADUP, &config.keybindDpadUp);
            break;
        case IDC_DPADDOWN:
            getConfigKey(hwndDlg, IDC_DPADDOWN, &config.keybindDpadDown);
            break;

        case IDC_MODIFIERKEY:
            getConfigKey(hwndDlg, IDC_MODIFIERKEY, &selectedKey);
            break;
        case IDC_MODIFIERADD:
            hDlgItem = GetDlgItem(hwndDlg, IDC_MODIFIERS);
            selectedIndex = SendMessage(hDlgItem, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
            if (selectedIndex >= 0) {
                config.modifiers[selectedIndex].keybind = selectedKey;
                config.modifiers[selectedIndex].multiplierX = selectedX;
                config.modifiers[selectedIndex].multiplierY = selectedY;
                setListRow(hwndDlg, selectedIndex, selectedKey, selectedX, selectedY);
            }
            break;
        case IDC_MODIFIERCLEAR:
            hDlgItem = GetDlgItem(hwndDlg, IDC_MODIFIERS);
            selectedIndex = SendMessage(hDlgItem, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
            if (selectedIndex >= 0) {
                config.modifiers[selectedIndex].keybind = 0;
                config.modifiers[selectedIndex].multiplierX = 0;
                config.modifiers[selectedIndex].multiplierY = 0;
                setListRow(hwndDlg, selectedIndex, 0, 0, 0);
            }
            break;
        }

    default:
        return FALSE;
    }
    return TRUE;
}

void OpenDialog(HINSTANCE hinst, HWND parent)
{
    parentVariable = parent;
    DialogBox(hinst, MAKEINTRESOURCE(IDD_DIALOG1), parent, DlgProc);
}

void getEditBoxContent(HWND hwndDlg, int nIDDlgItem, byte* returnVariable) {
    char lpch[4];
    hDlgItem = GetDlgItem(hwndDlg, nIDDlgItem);
    GetWindowTextA(hDlgItem, &lpch, sizeof(lpch));
    returnVariable = atoi(lpch);
    if (returnVariable > 127) {
        returnVariable = 127;
        setEditBoxContent(hwndDlg, nIDDlgItem, returnVariable);
    }
}

void getFloatEditBoxContent(HWND hwndDlg, int nIDDlgItem, float* returnVariable) {
    char lpch[32];
    hDlgItem = GetDlgItem(hwndDlg, nIDDlgItem);
    GetWindowTextA(hDlgItem, &lpch, sizeof(lpch));
    *returnVariable = atof(lpch);
    if (*returnVariable < 0) {
        *returnVariable = (0.0 - *returnVariable);
    }
    if (*returnVariable > 1.0) {
        *returnVariable = 1.0;
        setFloatEditBoxContent(hwndDlg, nIDDlgItem, returnVariable);
    }
}

void getConfigKey(HWND hwndDlg, int nIDDlgItem, byte* returnVariable) {
    int i, j;
    for (j = 0; j < 100; j++) {
        IDirectInputDevice8_GetDeviceState(lpdiKeyboard, (sizeof(deviceState)), (LPVOID*)&deviceState);
        deviceState[0] = 0;
        for (i = 0; i < sizeof(deviceState); i++) {
            if (deviceState[i] >> 7) {
                *returnVariable = i;
                setButtonLabel(hwndDlg, nIDDlgItem, *returnVariable);
                break;
            }
        }
        if (i < sizeof(deviceState)) {
            break;
        }
        Sleep(50);
    }
}

void setButtonLabel(HWND hwndDlg, int nIDDlgItem, byte returnVariable) {
    dips.diph.dwSize = sizeof(dips);
    dips.diph.dwHeaderSize = sizeof(diph);
    dips.diph.dwHow = DIPH_BYOFFSET;

    hDlgItem = GetDlgItem(hwndDlg, nIDDlgItem);
    dips.diph.dwObj = returnVariable;
    IDirectInputDevice8_GetProperty(lpdiKeyboard, DIPROP_KEYNAME, &dips); //this bich refuses to be ascii so gotta use unicode functions
    SetWindowTextW(hDlgItem, dips.wsz);
}

void setEditBoxContent(HWND hwndDlg, int nIDDlgItem, byte returnVariable) {
    char lpch[4];
    hDlgItem = GetDlgItem(hwndDlg, nIDDlgItem);
    Edit_LimitText(hDlgItem, 3);
    _itoa_s(returnVariable, lpch, sizeof(lpch), 10);
    SetWindowTextA(hDlgItem, lpch);
}

void setFloatEditBoxContent(HWND hwndDlg, int nIDDlgItem, float* returnVariable) {
    char lpch[32];
    errno_t err;
    hDlgItem = GetDlgItem(hwndDlg, nIDDlgItem);
    Edit_LimitText(hDlgItem, 8);
    err = _gcvt_s(lpch, sizeof(lpch), *returnVariable, 7);
    SetWindowTextA(hDlgItem, lpch);
}

void setListRow(HWND hwndDlg, int Index, int Key, float multX, float multY) {
    char lpch[sizeof(dips.wsz)];
    hDlgItem = GetDlgItem(hwndDlg, IDC_MODIFIERS);

    LvItem.iItem = Index;
    LvItem.iSubItem = 0;
    if (Key != 0) {
        dips.diph.dwSize = sizeof(dips);
        dips.diph.dwHeaderSize = sizeof(diph);
        dips.diph.dwHow = DIPH_BYOFFSET;
        dips.diph.dwObj = Key;
        IDirectInputDevice8_GetProperty(lpdiKeyboard, DIPROP_KEYNAME, &dips);
        LvItem.pszText = dips.wsz; //this bich refuses to be ascii so gotta use unicode functions
        SendMessageW(hDlgItem, LVM_SETITEMW, 0, (LPARAM)&LvItem);
    }
    else {
        LvItem.pszText = "Not set";
        SendMessageA(hDlgItem, LVM_SETITEMA, 0, (LPARAM)&LvItem);
    }

    LvItem.iSubItem = 1;
    _gcvt_s(lpch, sizeof(lpch), multX, 5);
    LvItem.pszText = lpch;
    SendMessageA(hDlgItem, LVM_SETITEMA, 0, (LPARAM)&LvItem);

    LvItem.iSubItem = 2;
    _gcvt_s(lpch, sizeof(lpch), multY, 5);
    LvItem.pszText = lpch;
    SendMessageA(hDlgItem, LVM_SETITEMA, 0, (LPARAM)&LvItem);
}

void resetButtonLabels(HWND hwndDlg) {
    setEditBoxContent(hwndDlg, IDC_CARDINALX, config.rangeCardinalX);
    setEditBoxContent(hwndDlg, IDC_CARDINALY, config.rangeCardinalY);
    setEditBoxContent(hwndDlg, IDC_DIAGONALX, config.rangeDiagonalX);
    setEditBoxContent(hwndDlg, IDC_DIAGONALY, config.rangeDiagonalY);

    setButtonLabel(hwndDlg, IDC_ABUTTON, config.keybindA);
    setButtonLabel(hwndDlg, IDC_BBUTTON, config.keybindB);
    setButtonLabel(hwndDlg, IDC_START, config.keybindStart);
    setButtonLabel(hwndDlg, IDC_LTRIG, config.keybindL);
    setButtonLabel(hwndDlg, IDC_ZTRIG, config.keybindZ);
    setButtonLabel(hwndDlg, IDC_RTRIG, config.keybindR);
    setButtonLabel(hwndDlg, IDC_ANALOGLEFT, config.keybindLeft);
    setButtonLabel(hwndDlg, IDC_ANALOGRIGHT, config.keybindRight);
    setButtonLabel(hwndDlg, IDC_ANALOGUP, config.keybindUp);
    setButtonLabel(hwndDlg, IDC_ANALOGDOWN, config.keybindDown);
    setButtonLabel(hwndDlg, IDC_CLEFT, config.keybindCLeft);
    setButtonLabel(hwndDlg, IDC_CRIGHT, config.keybindCRight);
    setButtonLabel(hwndDlg, IDC_CUP, config.keybindCUp);
    setButtonLabel(hwndDlg, IDC_CDOWN, config.keybindCDown);
    setButtonLabel(hwndDlg, IDC_DPADLEFT, config.keybindDpadLeft);
    setButtonLabel(hwndDlg, IDC_DPADRIGHT, config.keybindDpadRight);
    setButtonLabel(hwndDlg, IDC_DPADUP, config.keybindDpadUp);
    setButtonLabel(hwndDlg, IDC_DPADDOWN, config.keybindDpadDown);

    int i;
    for (i = 0; i < 50; i++) {
        setListRow(hwndDlg, i, config.modifiers[i].keybind, config.modifiers[i].multiplierX, config.modifiers[i].multiplierY);
    }
}