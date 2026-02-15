#pragma once

#include <Arduino.h>
#include "dma.h"

typedef struct
{
    volatile uint32_t completed_blocks;
    volatile uint32_t overrun_count;
    volatile uint32_t usb_tx_blocks;
    volatile uint32_t usb_tx_bytes;
    volatile uint32_t usb_tx_drop;
} capture_stats_t;

void capture_ring_init();
bool capture_ring_push(uint32_t block_index);
bool capture_pop_block(uint32_t* out_index);
void capture_mark_consumed(uint32_t index);
uint8_t* capture_block_ptr(uint32_t index);
uint32_t capture_block_size();
const capture_stats_t* capture_stats();
