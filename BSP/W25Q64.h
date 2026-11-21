#ifndef __W25Q64_H
#define __W25Q64_H

#include "main.h"
#include "spi.h"

/* 片选：探索者 V3 板上 25Q128 的 F_CS 接 PB14 */
#define W25Q64_CS_PIN    GPIO_PIN_14
#define W25Q64_CS_PORT   GPIOB

#define W25Q64_CS_LOW()    HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_RESET)
#define W25Q64_CS_HIGH()   HAL_GPIO_WritePin(W25Q64_CS_PORT, W25Q64_CS_PIN, GPIO_PIN_SET)

/* ID 说明：
 *   标准 Winbond W25Q128：JEDEC = 0xEF4018，厂商 0xEF、器件 0x17
 *   本板实测兼容片    ：JEDEC = 0x522118，厂商 0x52、器件 0x17（同为 128Mbit）
 *   因此型号判断以 0x90 读出的器件 ID(0x17) 为准，不看厂商号 */
#define W25Q128_ID       0xEF4018U
#define W25Q64_ID        0xEF4017U
#define W25Q_DEV_ID_128M 0x17U          /* 128Mbit 器件 ID */

#define W25Q64_SECTOR_SIZE   4096U      /* 最小擦除单位 4KB */
#define W25Q64_PAGE_SIZE     256U       /* 最大编程单位 256B */
#define W25Q64_CHIP_SIZE     0x1000000U /* 128Mbit = 16MB */

/* 命令字 */
#define W25Q_CMD_WRITE_EN    0x06U
#define W25Q_CMD_READ_SR1    0x05U
#define W25Q_CMD_READ_DATA   0x03U
#define W25Q_CMD_PAGE_PROG   0x02U
#define W25Q_CMD_SECTOR_ERA  0x20U
#define W25Q_CMD_JEDEC_ID    0x9FU
#define W25Q_CMD_DEV_ID      0xABU
#define W25Q_CMD_MANU_DEV_ID 0x90U

uint32_t W25Q64_ReadID(void);
uint8_t  W25Q64_ReadSR1(void);
void     W25Q64_WriteEnable(void);
void     W25Q64_WriteDisable(void);
void     W25Q64_WaitBusy(void);
uint8_t  W25Q64_Check(void);

void     W25Q64_ReadBuf(uint32_t addr, uint8_t *buf, uint16_t len);
void     W25Q64_PageWrite(uint32_t addr, const uint8_t *buf, uint16_t len);
void     W25Q64_SectorErase(uint32_t addr);

uint8_t  W25Q64_ReadDevID(void);
void     W25Q64_ReadManuDevID(uint8_t *p_manu, uint8_t *p_dev);

#endif
