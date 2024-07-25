#ifndef __KEY_H
#define __KEY_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

#define KEYPRESS	0x03
#define NoPRESS		0x04
#define KEY1PRESS	0x05
#define KEY2PRESS	0x06
#define KEY3PRESS	0x07
#define KEYAPRESS	0x08
#define KEYBPRESS	0x09
#define KEYDPRESS	0x10

void Init_Key_GPIO(void);
void Key_Sacnf(volatile struct Oscilloscope *value);
//void KEYD_SCAN(volatile struct Oscilloscope *value);
void Key_Handle(volatile struct Oscilloscope *value);
void Init_EC11_GPIO(void);

#endif