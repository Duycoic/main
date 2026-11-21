#include "main.h"
#include "sys.h"
#include "Delay.h"
#include "EXTI.h"
#include "UART.h"
#include "TIM.h"
#include "SPI.h"

#include "stdio.h"

#include "LED.h"
#include "KEY.h"
#include "W25Q64.h"

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	Delay_Init(168);

	LED_Init();
	KEY_Init();
	USART1_Init();

	Delay_ms(10);                           /* 等 Flash 上电稳定 */

	while (1)
	{
		LED_Toggle(LED0_PIN);               /* 心跳，说明主循环还活着 */
		Delay_ms(500);
	}
}
