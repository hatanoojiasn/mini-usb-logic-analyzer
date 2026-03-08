#include "capture_ring.h"

namespace
{
    constexpr uint32_t kRingSize = NumBrocks;
    constexpr uint32_t kBlockWords = BUF_WORDS;
    constexpr uint32_t kBlockBytes = kBlockWords * sizeof(uint32_t);

    volatile uint32_t ring_buffer[kRingSize];
    volatile uint32_t ring_head = 0;
    volatile uint32_t ring_tail = 0;
    volatile uint32_t ring_count = 0;

    capture_stats_t stats = {};
}

void capture_ring_init()
{
    noInterrupts();
    ring_head = 0;
    ring_tail = 0;
    ring_count = 0;
    stats = {};
    interrupts();
}

bool capture_ring_push(uint32_t block_index)
{
    bool pushed = false;
    noInterrupts();
    if (ring_count < kRingSize)
    {
        ring_buffer[ring_head] = block_index;
        ring_head = (ring_head + 1) % kRingSize;
        ring_count++;
        stats.completed_blocks++;
        pushed = true;
    }
    else
    {
        stats.overrun_count++;
    }
    interrupts();
    return pushed;
}

bool capture_pop_block(uint32_t* out_index)
{
    bool popped = false;
    noInterrupts();
    if (ring_count > 0)
    {
        *out_index = ring_buffer[ring_tail];
        ring_tail = (ring_tail + 1) % kRingSize;
        ring_count--;
        popped = true;
    }
    interrupts();
    return popped;
}

void capture_mark_consumed(uint32_t index)
{
    (void)index;
}

uint8_t* capture_block_ptr(uint32_t index)
{
    return reinterpret_cast<uint8_t*>(&buf_brocks[index][0]);
}

uint32_t capture_block_size()
{
    return kBlockBytes;
}

const capture_stats_t* capture_stats()
{
    return &stats;
}
