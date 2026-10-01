#ifndef __MAIN_H
#define __MAIN_H

/* exact-width unsigned integer types */

typedef signed int s32;
typedef signed short int s16;
typedef signed char s8;
typedef volatile unsigned int vu32;
typedef volatile unsigned short int vu16;
typedef volatile unsigned char vu8;
typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;
typedef const u32 uc32; /*!< Read Only */
typedef const u16 uc16; /*!< Read Only */
typedef const u8 uc8;   /*!< Read Only */

void Sys_Tick_Init();
void My_Delay_us(u32 n);
void My_Delay_ms(u32 n);
void All_Init();

#include "pin_resources.h"

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

#define vector3f Vector3f
#endif
