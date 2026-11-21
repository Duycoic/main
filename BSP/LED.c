#include "LED.h"

void LED_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PORT, LED0_PIN|LED1_PIN, GPIO_PIN_SET);

  /*Configure GPIO pins : PF9 PF10 */
  GPIO_InitStruct.Pin = LED0_PIN|LED1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

void LED_Toggle(uint16_t LED_PIN)
{
	HAL_GPIO_TogglePin(LED_PORT,LED_PIN);
}

void LED_ON(uint16_t LED_PIN)
{
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
}

void LED_OFF(uint16_t LED_PIN)
{
	HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
}
