#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LED_PORT GPIOF
#define LED0_PORT GPIOF
#define LED0_PIN GPIO_PIN_9
#define LED1_PORT GPIOF
#define LED1_PIN GPIO_PIN_10

void LED_Init(void);
void LED_Toggle(uint16_t LED_PIN);
void LED_ON(uint16_t LED_PIN);
void LED_OFF(uint16_t LED_PIN);

#endif /*__ GPIO_H__ */

