#ifndef __DELAY_H
#define __DELAY_H

#include "main.h"

/* 延时驱动：基于 DWT 周期计数器，提供 us / ms 级忙等延时 */
void Delay_Init(uint16_t sysclk);       /* 初始化时基，sysclk 为系统主频，单位 MHz */
void Delay_us(uint32_t nus);            /* 微秒延时 */
void Delay_ms(uint16_t nms);            /* 毫秒延时，参数上限 65535 ms */

#endif /* __DELAY_H */
