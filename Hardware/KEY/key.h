#ifndef __KEY_H
#define __KEY_H

#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"

#define KEYPRESS	0b00000001
#define NoPRESS		0b00000000
#define KEY1PRESS	0b00000010
#define KEY2PRESS	0b00000100
#define KEY3PRESS	0b00001000
#define KEYAPRESS	0b00010000
#define KEYBPRESS	0b00100000
#define KEYDPRESS	0b01000000

void Init_Key_GPIO(void);
void Key_Sacnf(volatile struct Oscilloscope *value);
void Key_Handle(volatile struct Oscilloscope *value);
void Init_EC11_GPIO(void);

#endif