#include "samd.h"
#include<Arduino.h>
#include "clock.h"
bool clock_init_gclk_dfll(int div)
{
    // GCLKとしてGCLK_IDで定義されたジェネレータを使用する
    GCLK->GENCTRL[GCLK_ID].reg = GCLK_GENCTRL_SRC_DFLL | // DFLLをソースに設定
                                 GCLK_GENCTRL_DIV(div) |// 分周比を設定
                                 GCLK_GENCTRL_GENEN;// ジェネレータを有効化
    GCLK->GENCTRL[GCLK_ID].bit.IDC = 1; // デューティ比を1:1に設定
    unsigned long t0 = millis();
    // GCLK_SYNCBUSY_GENCTRL() はマスク生成済みマクロ
    while(GCLK->SYNCBUSY.reg& GCLK_SYNCBUSY_GENCTRL(GCLK_ID))// GCLKの同期を待つ(GCLKのGENCTRLレジスタのGCLKで定義されたIDのビットがクリアされるまで待つ)
    {
         if (millis() - t0 > 50) { Serial.println("GENCTRL sync timeout"); return false; }
    }
    return GCLK->GENCTRL[GCLK_ID].bit.GENEN; // ジェネレータが有効化されたかを返す
    
}
bool clock_attach_tc_gclk(int pchctrl_tc_id, int gclk_id)
{
    // TCにGCLKを割り当てる
    GCLK->PCHCTRL[pchctrl_tc_id].reg = GCLK_PCHCTRL_GEN(gclk_id) | GCLK_PCHCTRL_CHEN;//GCLKIDをソースに設定し、クロックを有効化
    // クロックが有効化されるまで待つ
    unsigned long t0 = millis();
    while(!(GCLK->PCHCTRL[pchctrl_tc_id].reg & GCLK_PCHCTRL_CHEN))
    {
        if (millis() - t0 > 50) {
            Serial.println("PCHCTRL sync timeout");
            return false;
        }
    }
    return true;
}
void setup_clock()
{
    Serial.println("Setting up GCLK");
    bool is_setup_success = clock_init_gclk_dfll(CLOCK_DIV);//DCLKを有効にする　分周比を設定　DFLLを有効化　成功したかを返す
    if(!is_setup_success)
    {
        Serial.println("GCLK setup failed");
        return;
    }
    clock_attach_tc_gclk(GCLK_PCHCTRL_TC_ID,GCLK_ID);// TCにGCLKを割り当てる
    Serial.printf("GCLK setup complete\n");
}
