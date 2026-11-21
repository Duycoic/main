#include "UART.h"

#include "stdio.h"
#include "ringbuf.h"

UART_HandleTypeDef huart1;

// 发送环形缓冲区配置
#define UART_TX_RB_SIZE 256
static uint8_t UartTxBuf[UART_TX_RB_SIZE];
static ringbuf_t UartTxRb;
static bool UartTxBusy = false;  // 发送忙标记

static void UART_TxRingBuf_Init(void);

void USART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	UART_TxRingBuf_Init();

	HAL_NVIC_SetPriority(USART1_IRQn,4,0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
	Error_Handler();
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
  }
}

/**
 * @brief  串口发送环形缓冲初始化
 */
static void UART_TxRingBuf_Init(void)
{
    RingBuf_Init(&UartTxRb, UartTxBuf, UART_TX_RB_SIZE);
    UartTxBusy = false;
}

/**
 * @brief  从环形缓冲区取出数据，启动中断发送（批量版本，一次最多16字节，减少中断）
 */
#define TX_BATCH_MAX 16
static void UART_TxStart(void)
{
    static uint8_t tmp[TX_BATCH_MAX];
    uint16_t cnt = 0;

    if(RingBufIsEmpty(&UartTxRb))
    {
        UartTxBusy = false;
        return;
    }

    // 批量取出数据
    while(cnt < TX_BATCH_MAX && !RingBufIsEmpty(&UartTxRb))
    {
        RingBuf_Get(&UartTxRb, &tmp[cnt]);
        cnt++;
    }

    UartTxBusy = true;
    HAL_UART_Transmit_IT(&huart1, tmp, cnt);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        UART_TxStart();
    }
}

/**
 * @brief printf重定向，写入环形缓冲，非阻塞
 */
int fputc(int ch, FILE *f)
{
    // 写入环形缓冲区，满了直接丢弃
    if(RingBuf_Put(&UartTxRb, (uint8_t)ch))
    {
        if(UartTxBusy == false)
        {
            UART_TxStart();
        }
    }
    return ch;
}
