#include <Arduino.h>
#include "clock.h"
#include "tc.h"
#include "dma.h"
#include "capture_ring.h"
#define CAPTURE_PORT_GROUP 0
 uint8_t  GROUP = 0;        // PORTA
 uint8_t  PIN   = 10;
 uint32_t MASK  = (1u << PIN);
void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("DMA Ring Capture Start");
    setup_clock();
    setup_tc();
    setup_evesys_for_tc_dma();
    dma_global_enable();
    MCLK->APBBMASK.bit.PORT_ = 1;
    PORT->Group[GROUP].DIRCLR.reg = MASK;
    PORT->Group[GROUP].PINCFG[PIN].bit.INEN = 1;
    PORT->Group[GROUP].PINCFG[PIN].bit.PULLEN = 1;
    capture_ring_init();
    setup_dma_ringbuf(DMAC_CH_ID);
    // // --- データの準備 ---
    // // 転送元のデータ (buf[0]) に特徴的な値を入れる
    // buf[0] = 0xDEADBEEF;
    // // 転送先のデータをすべて0でクリアしておく
    // for (int i = 1; i < 16; i++) {
    //     buf[i] = 0;
    // }

    // Serial.println("--- Before DMA ---");
    // for (int i = 0; i < 16; i++) {
    //     Serial.printf("buf[%d] = 0x%08X\n", i, buf[i]);
    // }
    
    // // --- DMAの設定 ---
    // // buf[0] から buf[1] へ 15 ワードコピーする設定
    // setup_dma_for_memtest(&buf[0], &buf[1], 15);

    // // --- DMAの有効化とソフトウェアトリガ ---
    // // 1. テストに使うDMAチャネルを有効化
    // DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.ENABLE = 1;
    
    // // 2. ソフトウェアでトリガをかける
    // Serial.println("\nTriggering DMA via Software...");
    // DMAC->SWTRIGCTRL.reg = (1 << DMAC_CH_ID);

    // // 3. DMA転送が完了するのを待つ
    // // CHINTFLAG の TC (Transfer Complete) フラグが立つまで待機
    // while (!(DMAC->Channel[DMAC_CH_ID].CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL));
    
    // // 4. 完了フラグをクリアする（重要）
    // DMAC->Channel[DMAC_CH_ID].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
    
    // // 5. DMAチャネルを無効化
    // DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.ENABLE = 0;

    // Serial.println("DMA Transfer should be complete.");

    // // --- 結果の確認 ---
    // Serial.println("\n--- After DMA ---");
    // for (int i = 0; i < 16; i++) {
    //     Serial.printf("buf[%d] = 0x%08X\n", i, buf[i]);
    // }
}
void loop()
{
    static uint32_t last_log_ms = 0;
    uint32_t block_index = 0;
    if (capture_pop_block(&block_index))
    {
        uint8_t* block_ptr = capture_block_ptr(block_index);
        uint32_t block_size = capture_block_size();
        if (block_ptr != nullptr && block_size > 0)
        {
            const uint32_t sample = *reinterpret_cast<uint32_t*>(block_ptr);
            Serial.printf("block=%lu sample=0x%08lX\n", block_index, sample);
        }
        capture_mark_consumed(block_index);
    }

    const uint32_t now_ms = millis();
    if ((now_ms - last_log_ms) > 1000)
    {
        last_log_ms = now_ms;
        const capture_stats_t* stats = capture_stats();
        Serial.printf("completed=%lu overrun=%lu\n", stats->completed_blocks, stats->overrun_count);
    }
}
