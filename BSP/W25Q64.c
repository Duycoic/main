#include "w25q64.h"
#include "stdio.h"

/**
 * @brief  SPI1 全双工收发一个字节
 * @param  txData 待发送字节
 * @retval 同时收到的字节；通信失败时返回 0xFF
 * @note   超时 100ms，总线异常时不会把整机卡死
 */
static uint8_t W25Q64_SPI_ReadWriteByte(uint8_t txData)
{
    uint8_t rxData = 0xFF;

    if (HAL_SPI_TransmitReceive(&hspi1, &txData, &rxData, 1, 100) != HAL_OK)
    {
        return 0xFF;
    }
    return rxData;
}

/**
 * @brief  读取 JEDEC ID
 * @param  无
 * @retval 24bit ID；W25Q128 应为 0xEF4018
 * @note   命令 0x9F 之后连读 3 字节，全程 CS 保持低电平
 */
uint32_t W25Q64_ReadID(void)
{
    uint8_t id_buf[3];
    uint32_t id;

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_JEDEC_ID);
    id_buf[0] = W25Q64_SPI_ReadWriteByte(0xFF);
    id_buf[1] = W25Q64_SPI_ReadWriteByte(0xFF);
    id_buf[2] = W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_CS_HIGH();

    id = ((uint32_t)id_buf[0] << 16) | ((uint32_t)id_buf[1] << 8) | (uint32_t)id_buf[2];
    return id;
}

/**
 * @brief  读状态寄存器 1
 * @param  无
 * @retval SR1 值，bit0 = BUSY，bit1 = WEL
 */
uint8_t W25Q64_ReadSR1(void)
{
    uint8_t sr;

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_READ_SR1);
    sr = W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_CS_HIGH();

    return sr;
}

/**
 * @brief  写使能，置 WEL 位
 * @param  无
 * @retval 无
 * @note   擦除/编程前必须先调用
 */
void W25Q64_WriteEnable(void)
{
    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_WRITE_EN);
    W25Q64_CS_HIGH();
}

/**
 * @brief  写禁止，清 WEL 位
 * @param  无
 * @retval 无
 */
void W25Q64_WriteDisable(void)
{
    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(0x04);
    W25Q64_CS_HIGH();
}

/**
 * @brief  等待芯片内部擦写完成
 * @param  无
 * @retval 无
 * @note   轮询 SR1 的 BUSY 位；带超时保护，防止芯片异常时死等
 */
void W25Q64_WaitBusy(void)
{
    uint32_t timeout = 0x00080000U;

    while (((W25Q64_ReadSR1() & 0x01U) != 0U) && (timeout != 0U))
    {
        timeout--;
    }
}

/**
 * @brief  从指定地址连续读数据
 * @param  addr 起始地址，24bit
 * @param  buf  接收缓冲区
 * @param  len  读取字节数
 * @retval 无
 * @note   命令 0x03，读取期间 CS 保持低电平
 */
void W25Q64_ReadBuf(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_READ_DATA);
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 16));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 8));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr));
    for (i = 0; i < len; i++)
    {
        buf[i] = W25Q64_SPI_ReadWriteByte(0xFF);   /* 空写 0xFF 产生时钟 */
    }
    W25Q64_CS_HIGH();
}

/**
 * @brief  页编程，一次最多 256 字节
 * @param  addr 起始地址，24bit
 * @param  buf  待写数据
 * @param  len  字节数，不超过 256 且不得跨页
 * @retval 无
 * @note   内部先写使能，写完等待 BUSY 清零，阻塞约 1~3ms
 */
void W25Q64_PageWrite(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    W25Q64_WriteEnable();

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_PAGE_PROG);
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 16));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 8));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr));
    for (i = 0; i < len; i++)
    {
        W25Q64_SPI_ReadWriteByte(buf[i]);
    }
    W25Q64_CS_HIGH();

    W25Q64_WaitBusy();
}

/**
 * @brief  擦除一个 4KB 扇区
 * @param  addr 扇区内任意地址，24bit
 * @retval 无
 * @note   擦除后该扇区全为 0xFF，耗时约 45ms，期间阻塞
 */
void W25Q64_SectorErase(uint32_t addr)
{
    W25Q64_WriteEnable();

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_SECTOR_ERA);
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 16));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr >> 8));
    W25Q64_SPI_ReadWriteByte((uint8_t)(addr));
    W25Q64_CS_HIGH();

    W25Q64_WaitBusy();
}

/**
 * @brief  判断 Flash 是否能正常识别
 * @param  无
 * @retval 1 = 正常（0x90 的器件 ID 为 0x17，128Mbit），0 = 异常
 * @note   兼容片的厂商号可能是 0x52 等非 Winbond 值，故只校验器件 ID
 */
uint8_t W25Q64_Check(void)
{
    uint8_t manu = 0;
    uint8_t dev  = 0;

    W25Q64_ReadManuDevID(&manu, &dev);
    (void)manu;

    return (dev == W25Q_DEV_ID_128M) ? 1U : 0U;
}

/**
 * @brief  读器件 ID（旧式 0xAB 命令）
 * @param  无
 * @retval 器件 ID，W25Q128 应为 0x17
 * @note   与 0x9F 交叉验证用；命令后需 3 个空字节
 */
uint8_t W25Q64_ReadDevID(void)
{
    uint8_t id;

    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_DEV_ID);
    W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_SPI_ReadWriteByte(0xFF);
    id = W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_CS_HIGH();

    return id;
}

/**
 * @brief  读厂商 ID + 器件 ID（0x90 命令）
 * @param  p_manu 输出厂商 ID；p_dev 输出器件 ID
 * @retval 无
 * @note   命令后带 3 字节地址 00 00 00；W25Q128 应为 0xEF / 0x17
 */
void W25Q64_ReadManuDevID(uint8_t *p_manu, uint8_t *p_dev)
{
    W25Q64_CS_LOW();
    W25Q64_SPI_ReadWriteByte(W25Q_CMD_MANU_DEV_ID);
    W25Q64_SPI_ReadWriteByte(0x00);
    W25Q64_SPI_ReadWriteByte(0x00);
    W25Q64_SPI_ReadWriteByte(0x00);
    *p_manu = W25Q64_SPI_ReadWriteByte(0xFF);
    *p_dev  = W25Q64_SPI_ReadWriteByte(0xFF);
    W25Q64_CS_HIGH();
}
