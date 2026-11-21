#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define KEY_RIGHT     GPIO_PIN_4
#define KEY_DOWN     GPIO_PIN_3
#define KEY_LEFT     GPIO_PIN_2
#define KEY_UP     GPIO_PIN_0

#define KEY_PRESSED    GPIO_PIN_RESET
#define KEY_RELEASED   GPIO_PIN_SET

/* 按键扫描状态机 */
typedef enum
{
    KEY_STATE_IDLE,
    KEY_STATE_DEBOUNCE,
    KEY_STATE_PRESSED,
    KEY_STATE_WAIT_RELEASE
} Key_State_t;

/* 按键编码 */
typedef enum
{
    KEY_NONE  = 0, 
    KEY_RIGHT_C = 1, //KEY0
    KEY_DOWN_C = 2,	   //KEY1
    KEY_LEFT_C = 3,  //KEY2
    KEY_UP_C = 4   //WKUP
} Key_Code_t;

void KEY_Init(void); 
Key_Code_t Key_Scan(void);     /* 扫描一次按键，无按键时返回 KEY_NONE */

#endif /* __KEY_H */
