#include "dma.h"
#include "samd.h"
#include "clock.h"
uint32_t buf[BUF_WORDS];
static DmacDescriptor dma_desc __attribute__((aligned(16),section(".desc")));
void setup_dma()
{
    Serial.println("Setting up DMA...");
    setup_evesys();
    MCLK->AHBMASK.bit.DMAC_ = 1;//読み込み元を設定
    Serial.printf("DMAC = %d\n", MCLK->AHBMASK.bit.DMAC_ );
    dma_desc.BTCTRL.reg = DMAC_BTCTRL_VALID |
                            DMAC_BTCTRL_DSTINC|
                         DMAC_BTCTRL_EVOSEL_DISABLE |
                         DMAC_BTCTRL_BLOCKACT_INT |
                         DMAC_BTCTRL_BEATSIZE_WORD;
    dma_desc.DSTADDR.reg = (uint32_t) &buf[0];//書き込み先を設定
    dma_desc.SRCADDR.reg = (uint32_t)PORT->Group[0].OUT.reg;//読み込み元を設定
    dma_desc.BTCNT.reg = BUF_WORDS;//転送するデータ数を設定
    dma_desc.DESCADDR.reg = (uint32_t)&dma_desc;
    //DMAチャンネルの設定
    DMAC->BASEADDR.reg = (uint32_t)&dma_desc;//リングバッファの参照先指定　自己参照なので無限ループ
    DMAC->Channel[0].CHCTRLA.reg = DMAC_CHCTRLA_ENABLE|
                                   DMAC_CHCTRLA_TRIGSRC(EVSYS_ID_GEN_TC0_MCX_0)|// TC0のMCX0をトリガ
                                    DMAC_CHCTRLA_TRIGACT_BURST;//バースト転送
                                    Serial.printf("DMAC setup complete\n");
}
void setup_evesys()
{
    EVSYS->USER[EVSYS_ID_USER_DMAC_CH_0].reg = EVSYS_USER_CHANNEL(0);
    EVSYS->Channel[0].CHANNEL.reg = EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TC0_MCX_0)|
                                    EVSYS_CHANNEL_PATH_SYNCHRONOUS|
                                    EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT;
                                    Serial.println("EVSYS setup complete");
}

