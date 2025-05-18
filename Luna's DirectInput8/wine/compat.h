#pragma once

#define DIDFT_OPTIONAL          0x80000000

#define DIDEVTYPE_DEVICE                1
#define DIDEVTYPE_MOUSE                 2
#define DIDEVTYPE_KEYBOARD              3
#define DIDEVTYPE_JOYSTICK              4
#define DIDEVTYPE_HID                   0x00010000

#define DIDEVTYPEMOUSE_TRADITIONAL      2

#define DIDEVTYPEKEYBOARD_PCENH         4
#define DIDEVTYPEKEYBOARD_JAPAN106     10

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#endif
