#pragma once
#include <Arduino.h>
#include "samd.h"
#define EVESYS_USER_CH_ID_0 1 // EVSYSのUSERCHID0
#define EVESYS_USER_ID 0 // EVSYSのUSERID
#define EVESYS_USER_DMAC_CH_ID EVSYS_ID_USER_DMAC_CH_0//EVESYSを使用するDMCAのチャネルを設定
#define EVESYS_GEN_ID EVSYS_ID_GEN_TC0_MCX_0
#define EVESYS_CH_ID 0 // EVSYSのCHID
#define EVESYS_USER_CH_ID EVESYS_USER_CH_ID_0
#define BUF_WORDS 256 // 例: バッファサイズ
#define DMAC_CH_ID 0 // 使用するDMACチャネル
#define DMAC_CH_GROUP 0 // 使用するDMACチャネルグループ
#define PORTGROUP 0 // 使用するポートグループ
#define DMA_TRIGSRC_TC0_MC0   TC0_DMAC_ID_MC_1
#define NumBrocks 2 // リングバッファのブロック数
#define DMA_CH_NUM 1 // 使用するDMAチャネル数
extern uint32_t buf_brocks [NumBrocks][BUF_WORDS];
extern uint32_t buf[BUF_WORDS];
extern DmacDescriptor dma_desc    __attribute__((aligned(16)));
extern DmacDescriptor dma_desc_wb[DMA_CH_NUM] __attribute__((aligned(16)));
extern DmacDescriptor dma_descs[NumBrocks] __attribute__((aligned(16)));
extern DmacDescriptor channel_dma_desc[DMA_CH_NUM] __attribute__((aligned(16)));
typedef enum{
    DMA_TRIG_EVESYS,
    DMA_TRIG_PERIPHERAL,
} dma_trig_type;
void dma_global_enable();
bool  dma_mem2mem_once(int src, int dst,int words);
bool dma_gpio_sample_once(int reg,int dst,int words);
bool dma_ch_wait_done(int timeout_ms);
void setup_dma_for_memtest(const void* src, void* dst, uint32_t words);
void setup_evesys_for_tc_dma();
void setup_dma_ringbuf(uint8_t ch_id);
void dma_channel_reset(uint8_t ch_id);
void dma_channel_common_init(uint8_t ch_id);
void dma_channel_set_trig(uint8_t ch_id, dma_trig_type trig_type);
void dma_channel_enable(uint8_t ch_id);
