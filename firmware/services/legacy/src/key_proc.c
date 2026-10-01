#include "key_proc.h"
#include "board_io.h"

extern u8 LcdPage;

osThreadId KeyTaskHandle;

u8 KeyVal, KeyOld, KeyUp, KeyDown;

void Key_Task_Proc(void const *argument) {

    for (;;) {

        KeyVal = Key_Scan();
        KeyDown = KeyVal & (KeyVal ^ KeyOld);
        KeyUp = ~KeyVal & (KeyVal ^ KeyOld);
        KeyOld = KeyVal;
        //	printf("vol:%f\r\n", volReal);

        if (KeyDown == 1) {
            //		printf("key1 down\r\n");
        } else if (KeyDown == 2) {
            //		printf("key2 down\r\n");
        }
        osDelay(50);
    }
}

u8 Key_Scan(void) { return maoxiu_board_key_scan(); }
