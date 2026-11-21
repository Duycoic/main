#ifndef __RINGBUF_H
#define __RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t *buffer;
    uint16_t buf_size;
    uint16_t wr_idx;
    uint16_t rd_idx;
} ringbuf_t;

void RingBuf_Init(ringbuf_t *rb, uint8_t *buf, uint16_t buf_size);
bool RingBuf_Put(ringbuf_t *rb, uint8_t ch);
bool RingBuf_Get(ringbuf_t *rb, uint8_t *ch);
uint16_t RingBuf_Write(ringbuf_t *rb, const uint8_t *data, uint16_t len);
uint16_t RingBuf_Read(ringbuf_t *rb, uint8_t *data, uint16_t len);
uint16_t RingBuf_GetCount(ringbuf_t *rb);
uint16_t RingBuf_GetFreeSpace(ringbuf_t *rb);
bool RingBufIsEmpty(ringbuf_t *rb);
bool RingBufIsFull(ringbuf_t *rb);
void RingBuf_Clear(ringbuf_t *rb);

#endif
