#include "Delay.h"

/* DWT 周期换算系数：等于系统主频，单位 MHz，即 1 us 对应的计数值 */
static uint32_t s_fac_us = 0;

/**
 * @brief  初始化 DWT 周期计数器，为延时提供时基
 * @param  sysclk 系统主频，单位 MHz
 * @retval 无
 * @note   须先于 delay_us/delay_ms 调用一次；会清零并使能 CYCCNT
 */
void Delay_Init(uint16_t sysclk)
{
    s_fac_us = sysclk;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     /* 使能 DWT 跟踪单元 */
    DWT->CYCCNT = 0;
    DWT->CTRL   |= DWT_CTRL_CYCCNTENA_Msk;              /* 启动周期计数器 */
}

/**
 * @brief  微秒级忙等延时
 * @param  nus 延时长度，单位 us
 * @retval 无
 * @note   轮询 DWT 周期计数，阻塞且不可重入；精度取决于 delay_init 传入的主频
 */
void Delay_us(uint32_t nus)
{
    uint32_t start = DWT->CYCCNT;                       /* 记录起始计数值 */
    uint32_t ticks = nus * s_fac_us;

    while ((DWT->CYCCNT - start) < ticks)               /* 无符号差值比较，规避计数器回绕 */
    {
        /* 空循环等待 */
    }
}

/* 毫秒延时：换算为微秒后复用 delay_us */
void Delay_ms(uint16_t nms)
{
    Delay_us((uint32_t)nms * 1000U);
}

/**
 * @brief  重写 HAL 库毫秒延时，改用 DWT 实现
 * @param  delay 延时长度，单位 ms
 * @retval 无
 * @note   按 60000 ms 分段，避免强转 uint16_t 时溢出；阻塞式，不进入低功耗
 */
void HAL_Delay(uint32_t delay)
{
    while (delay > 0U)
    {
        uint32_t step = (delay > 60000U) ? 60000U : delay;  /* 单次不超出 uint16_t 上限 */
        Delay_ms((uint16_t)step);
        delay -= step;
    }
}
