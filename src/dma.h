#pragma once
#include <Arduino.h>
#define BUF_WORDS 256 // 例: バッファサイズ
extern uint32_t buf[BUF_WORDS]; // DMA転送用バッファ
// ...existing code...
void setup_dma();
void setup_evesys();