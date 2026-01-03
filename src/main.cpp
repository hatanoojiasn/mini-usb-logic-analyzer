#include <Arduino.h>
#include "clock.h"
#include "tc.h"
#include "dma.h"
#define CAPTURE_PORT_GROUP 0
 uint8_t  GROUP = 0;        // PORTA
 uint8_t  PIN   = 10;
 uint32_t MASK  = (1u << PIN);
void setup()
{
    /*
    Serial.begin(115200);
    while(!Serial);
    Serial.println("DMA Test1");
    setup_clock();
    setup_tc();
    dma_global_enable();
       Serial.println("SetupEnd");

  dma_gpio_sample_once(0,0,256);
// 1) ポート準備
MCLK->APBBMASK.bit.PORT_ = 1;
PORT->Group[GROUP].DIRCLR.reg = MASK;        // 入力方向
PORT->Group[GROUP].PINCFG[PIN].bit.INEN = 1; // 入力バッファON
PORT->Group[GROUP].PINCFG[PIN].bit.PULLEN = 1;
PORT->Group[CAPTURE_PORT_GROUP].DIRSET.reg = MASK;       // 出力化
PORT->Group[CAPTURE_PORT_GROUP].OUTCLR.reg = MASK;       // 初期Low
*/
Serial.begin(115200);
    while (!Serial);
    Serial.println("DMA Memory Test Start");
    setup_clock();
    setup_tc();
    setup_evesys_for_tc_dma();
    dma_global_enable();

while(1){
dma_gpio_sample_once(0, 0, 16);  // 1 回だけ有効化

while(!(DMAC->Channel[DMAC_CH_ID].CHINTFLAG.reg & DMAC_CHINTFLAG_TCMPL));
DMAC->Channel[DMAC_CH_ID].CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; // フラグクリア

// 結果表示
for (int i = 0; i < 16; i++) {
    Serial.printf("buf[%d]=%08lX\n", i, buf[i]);
}
}
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
       /* static uint32_t tick = 0;
    if ((tick++ & 0x7FFF) == 0) {                     // 適当な周期
        PORT->Group[CAPTURE_PORT_GROUP].OUTTGL.reg = MASK;
    }
        for(int i=0;i<16;i++)
    {
        Serial.printf("buf[%d]=%08X\n",i,buf[i]);
    }
  
    delay(1000);
*/
}
