#include "ringbuf.h"

/*把一个数组初始化为一个环形缓冲区*/
void RingBuf_Init(ringbuf_t *rb, uint8_t *buf, uint16_t buf_size)
{
    rb->buffer  = buf;
    rb->buf_size = buf_size;
    rb->wr_idx  = 0;
    rb->rd_idx  = 0;
}

/**
 * @brief 写入单字节
 * @retval true成功，false缓冲区满
 */
bool RingBuf_Put(ringbuf_t *rb, uint8_t ch)
{
    if(RingBufIsFull(rb))
    {
        return false;
    }
    rb->buffer[rb->wr_idx] = ch;
    rb->wr_idx = (rb->wr_idx + 1) % rb->buf_size;
    return true;
}

/**
 * @brief 读取单字节
 * @param ch 输出读到的数据
 * @retval true成功，false缓冲区空
 */
bool RingBuf_Get(ringbuf_t *rb, uint8_t *ch)
{
    if(RingBufIsEmpty(rb))
    {
        return false;
    }
    *ch = rb->buffer[rb->rd_idx];
    rb->rd_idx = (rb->rd_idx + 1) % rb->buf_size;
    return true;
}

/**
 * @brief 批量写入多个字节
 * @return 实际成功写入字节数
 */
uint16_t RingBuf_Write(ringbuf_t *rb, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t write_num = 0;
    for(i = 0; i < len; i++)
    {
        if(RingBuf_Put(rb, data[i]))
        {
            write_num++;
        }
        else
        {
            break;
        }
    }
    return write_num;
}

/**
 * @brief 批量读取多个字节
 * @return 实际读到的字节数
 */
uint16_t RingBuf_Read(ringbuf_t *rb, uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t read_num = 0;
    for(i = 0; i < len; i++)
    {
        if(RingBuf_Get(rb, &data[i]))
        {
            read_num++;
        }
        else
        {
            break;
        }
    }
    return read_num;
}

/**
 * @brief 获取当前有效数据个数
 */
uint16_t RingBuf_GetCount(ringbuf_t *rb)
{
    if(rb->wr_idx >= rb->rd_idx)
    {
        return rb->wr_idx - rb->rd_idx;
    }
    else
    {
        return (rb->buf_size - rb->rd_idx) + rb->wr_idx;
    }
}

/**
 * @brief 获取剩余可写空间
 */
uint16_t RingBuf_GetFreeSpace(ringbuf_t *rb)
{
    return (rb->buf_size - 1U) - RingBuf_GetCount(rb);
}

bool RingBufIsEmpty(ringbuf_t *rb)
{
    return rb->wr_idx == rb->rd_idx;
}

bool RingBufIsFull(ringbuf_t *rb)
{
    return ((rb->wr_idx + 1U) % rb->buf_size) == rb->rd_idx;
}

/**
 * @brief 清空环形缓冲区
 */
void RingBuf_Clear(ringbuf_t *rb)
{
    rb->wr_idx = 0;
    rb->rd_idx = 0;
}

