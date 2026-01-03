#include "dma.h"
#include "samd.h"
#include "clock.h"
uint32_t buf[BUF_WORDS];
static DmacDescriptor dma_desc    __attribute__((aligned(16)));
static DmacDescriptor dma_desc_wb __attribute__((aligned(16))); // Write-Back用(必須)
static uint32_t buf_brocks [NumBrocks][BUF_WORDS];
static DmacDescriptor channel_dam_desc[DMA_CH_NUM] __attribute__((aligned(16)));
static DmacDescriptor dma_descs[NumBrocks] __attribute__((aligned(16)));
void dma_global_enable()
{
    Serial.println("Setting up DMA...");

    MCLK->AHBMASK.bit.DMAC_ = 1; // クロック有効化
    
    // DMACのリセットと無効化
    DMAC->CTRL.bit.DMAENABLE = 0;
    DMAC->CTRL.bit.SWRST = 1;
    while (DMAC->CTRL.bit.SWRST);

    // ディスクリプタのアドレス設定
    DMAC->BASEADDR.reg = (uint32_t)&dma_desc;
    DMAC->WRBADDR.reg  = (uint32_t)&dma_desc_wb; // ★ここが必須

    // 全プライオリティレベルを有効化してDMA有効化
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xF);

    Serial.printf("DMAC setup complete. BASE=0x%08lX WB=0x%08lX\n", (uint32_t)&dma_desc, (uint32_t)&dma_desc_wb);
}

bool dma_mem2mem_once(int src, int dst,int words)
{
    // 1. チャネルを無効化してリセット
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.ENABLE);
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.SWRST);

    // 2. チャネルを有効化して設定
    dma_desc.BTCTRL.reg = DMAC_BTCTRL_VALID |
                          DMAC_BTCTRL_DSTINC|
                          DMAC_BTCTRL_SRCINC|
                          DMAC_BTCTRL_EVOSEL_DISABLE|
                          DMAC_BTCTRL_BLOCKACT_INT |
                          DMAC_BTCTRL_BEATSIZE_WORD;
    dma_desc.BTCNT.reg = words;
    dma_desc.SRCADDR.reg = (uint32_t) &buf[src+words];//読み込み元を設定
    dma_desc.DSTADDR.reg = (uint32_t) &buf[dst+words];//書き込み先を設定
    dma_desc.BTCNT.reg = words;//転送するデータ数を設定

    DMAC->Channel[DMAC_CH_ID].CHEVCTRL.reg = DMAC_CHEVCTRL_EVIE|
    DMAC_CHEVCTRL_EVACT_TRIG; // イベント有効化

    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = 
    DMAC_CHCTRLA_TRIGSRC(DMAC_CHCTRLA_TRIGSRC_DISABLE_Val) | // TC0 MC0 トリガー
    DMAC_CHCTRLA_TRIGACT_BLOCK |             // トリガでブロック転送
    DMAC_CHCTRLA_ENABLE;                     // 有効化
    return true;
}
bool dma_gpio_sample_once(int reg, int dst, int words)
{
    if (words > BUF_WORDS) words = BUF_WORDS;

    // 1. チャネルを無効化してリセット
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.ENABLE);
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.SWRST);

    // 2. ディスクリプタの設定
    dma_desc.BTCTRL.reg = DMAC_BTCTRL_VALID |
                          DMAC_BTCTRL_DSTINC |        // 書き込み先(メモリ)はインクリメント
                          // SRCINCは無し (GPIOは固定アドレス)
                          DMAC_BTCTRL_BEATSIZE_WORD | // 32bit転送
                          DMAC_BTCTRL_BLOCKACT_INT;   // 完了後に割り込みフラグを立てて停止

    dma_desc.BTCNT.reg = words; // 転送回数

    // 読み込み元: GPIO入力レジスタ
    dma_desc.SRCADDR.reg = (uint32_t)&PORT->Group[PORTGROUP].IN.reg; 

    // 書き込み先: bufの指定位置
    // SAMD51では「最後のビートが書き込まれるアドレス」を指定する
    // つまり StartAddr + (Count * Size)
    dma_desc.DSTADDR.reg = (uint32_t)&buf[dst] + (words * sizeof(uint32_t));

    dma_desc.DESCADDR.reg = 0; // リンクなし

    DMAC->Channel[DMAC_CH_ID].CHEVCTRL.reg = DMAC_CHEVCTRL_EVIE|
    DMAC_CHEVCTRL_EVACT_TRIG; // イベント有効化
    // 3. チャネルの設定と有効化
    // ★重要: ENABLEするときに TRIGSRC と TRIGACT を同時に設定する
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = 
        DMAC_CHCTRLA_TRIGSRC(DMAC_CHCTRLA_TRIGSRC_DISABLE_Val) | // 
        DMAC_CHCTRLA_TRIGACT_BURST |             // 1トリガで1バースト(今回は1ワード)転送
        DMAC_CHCTRLA_BURSTLEN_SINGLE |           // バースト長は1
        DMAC_CHCTRLA_ENABLE;                     // 有効化
    return true;
}
void setup_dma_for_memtest(const void* src, void* dst, uint32_t words)
{
    Serial.println("Setting up DMA for Memory Test...");

    // 1. DMACのクロックを有効化
    MCLK->AHBMASK.bit.DMAC_ = 1;

    // 2. DMAディスクリプタ（転送内容）の設定
    dma_desc.BTCTRL.reg = DMAC_BTCTRL_VALID |         // ディスクリプタを有効に
                          DMAC_BTCTRL_DSTINC |        // 宛先アドレスはインクリメントする
                        //DMAC_BTCTRL_SRCINC |        // ★ソースアドレスはインクリメントしない
                          DMAC_BTCTRL_BEATSIZE_WORD;  // 転送単位はワード(32bit)

    dma_desc.BTCNT.reg = words;                                 // 転送するワード数
    dma_desc.SRCADDR.reg = (uint32_t)src;                        // 転送元アドレス
    dma_desc.DSTADDR.reg = (uint32_t)dst + (words * sizeof(uint32_t)); // 転送完了後の宛先アドレス
    dma_desc.DESCADDR.reg = 0;                                  // 次のディスクリプタはない

    DMAC->Channel[DMAC_CH_ID].CHEVCTRL.reg = DMAC_CHEVCTRL_EVIE|
    DMAC_CHEVCTRL_EVACT_TRIG; // イベント有効化

    // 3. DMACの基本設定
    DMAC->BASEADDR.reg = (uint32_t)&dma_desc; // ディスクリプタのアドレスを設定
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE;     // DMAを有効化

    // 4. DMAチャネルの設定
    //    チャネルはこの時点では有効化しない
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = 
        DMAC_CHCTRLA_TRIGSRC(DMAC_CHCTRLA_TRIGSRC_DISABLE_Val) |                 // ソフトウェアトリガ
        DMAC_CHCTRLA_TRIGACT_BLOCK;         // ★トリガソース: 0x00 (ソフトウェアトリガのみ)
    
    Serial.println("DMA Memory Test setup complete.");
}

void setup_evesys_for_tc_dma()
{
    Serial.println("Setting up Event System for TC DMA Trigger...");

    // 1. EVSYSのクロックを有効化
    MCLK->APBBMASK.bit.EVSYS_ = 1;

    // 2. イベントジェネレータの設定: TC0 MC0 を使用
    EVSYS->USER[EVESYS_USER_DMAC_CH_ID].reg = 
        EVSYS_USER_CHANNEL(EVESYS_USER_CH_ID);    // チャネル0からイベントを受け取る

    // 3. イベントチャネルの設定
    EVSYS->Channel[EVESYS_CH_ID].CHANNEL.reg =EVSYS_CHANNEL_EVGEN(EVESYS_GEN_ID)|
                                                EVSYS_CHANNEL_PATH_ASYNCHRONOUS|
                                                EVSYS_CHANNEL_EDGSEL_RISING_EDGE;
                                            
    Serial.println("Event System setup complete.");
}
void setup_dma_ringbuf()
{
    Serial.println("SETUP DMA RING BUF");
    // 1. チャネルを無効化してリセット
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.ENABLE);
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = 0;
    while (DMAC->Channel[DMAC_CH_ID].CHCTRLA.bit.SWRST);
// 2. チャネルを有効化して設定
    for(int i=0;i<NumBrocks;i++)
    {
        dma_descs[i].SRCADDR.reg =  (uint32_t)&PORT->Group[PORTGROUP].IN.reg + beatsize_byte; //読み込み元を設定
        dma_descs[i].DSTADDR.reg = (uint32_t)&buf_brocks[i][BUF_WORDS]; //書き込み先を設定
        dma_descs[i].BTCTRL.reg  = DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_BEATSIZE_WORD | DMAC_BTCTRL_BLOCKACT_INT;
        dma_descs[i].DESCADDR.reg = (uint32_t)&dma_descs[(i+1)%NumBrocks];
        dma_descs[i].BTCNT.reg = BUF_WORDS;//転送するデータ数を設定
        dma_descs[i].BTCTRL.bit.VALID = 1;
    }
    for (int i = 0; i < DMA_CH_NUM; i++)
    {
        channel_dam_desc[i].DESCADDR.reg = (uint32_t)&dma_descs[0];
        channel_dam_desc[i].BTCNT.reg = BUF_WORDS;
        channel_dam_desc[i].BTCTRL.bit.VALID = 1;
    }
    DMAC->Channel[DMAC_CH_ID].CHEVCTRL.reg = DMAC_CHEVCTRL_EVIE | DMAC_CHEVCTRL_EVACT_TRIG;
    DMAC->BASEADDR.reg = (uint32_t)&channel_dam_desc[0];
    DMAC->WRBADDR.reg = (uint32_t)&dma_desc_wb; 
    DMAC->Channel[DMAC_CH_ID].CHCTRLA.reg = 
        DMAC_CHCTRLA_TRIGSRC(DMAC_CHCTRLA_TRIGSRC_DISABLE_Val) |
        DMAC_CHCTRLA_TRIGACT_TRANSACTION |         // トリガでトランザクション転送
        DMAC_CHCTRLA_ENABLE;                     // 有効化
    Serial.println("SETUP DMA RING BUF END");

}