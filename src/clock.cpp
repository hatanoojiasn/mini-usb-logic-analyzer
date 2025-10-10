#include "samd.h"
#include<Arduino.h>
#include "clock.h"
void setup_clock()
{
    Serial.println("Setting up clock...1");
    GCLK->GENCTRL[3].bit.GENEN =0;
    while(GCLK->SYNCBUSY.bit.GENCTRL3) // Wait for synchronization
    {
        
    }
    // DFLLを有効化
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_DIV(3) | GCLK_GENCTRL_GENEN;
    GCLK->GENCTRL[3].bit.IDC = 1; // Improve Duty Cycle
    unsigned long t0 = millis();
    while(GCLK->SYNCBUSY.bit.GENCTRL3)// Wait for synchronization
    {
         if (millis() - t0 > 50) { Serial.println("GENCTRL1 sync timeout"); break; }
    }
    
    Serial.println("Setting up clock...2");
    // TC0にGCLK1を割り当て
    GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    // Wait for the clock to be enabled
    t0 = millis();
    while(!(GCLK->PCHCTRL[TC0_GCLK_ID].reg & GCLK_PCHCTRL_CHEN))
    {
        if (millis() - t0 > 50) {
            Serial.println("PCHCTRL sync timeout");
            break;
        }
    }
    Serial.println("Setting up clock...3");
    //TC0の設定
    TC0->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1 | TC_WAVE_WAVEGEN_MFRQ;
    TC0->COUNT16.CC[0].reg = 1000;
    TC0->COUNT16.EVCTRL.bit.MCEO0 = 1;
    TC0->COUNT16.CTRLA.bit.ENABLE = 1;
    Serial.printf("Clock setup complete\n");
}
