#include "tc.h"
#define GCLK_ID 3 // TC0に接続するGCLKのID
//分周比とカウンタ値を設定 成功したかを返す
bool tc_config_mfrq(int presc_val,int cc0)
{
  MCLK->APBAMASK.bit.TC0_ = 1;//TC0のクロックを有効化
  auto p = GCLK->PCHCTRL[TC0_GCLK_ID].reg;
  Serial.printf("PCHCTRL(TC0)=0x%08lX CHEN=%u GEN=%u\n",
              p,
              (unsigned)((p & GCLK_PCHCTRL_CHEN) != 0),
              (unsigned)((p & GCLK_PCHCTRL_GEN_Msk) >> GCLK_PCHCTRL_GEN_Pos));
  Serial.println("tc_config_mfrq_start");
    TC0->COUNT16.CTRLA.bit.ENABLE=false;//TCを無効化
    while (TC0->COUNT16.SYNCBUSY.bit.ENABLE); 
     Serial.println("tc_SYNCBUSY_wait_end");
    TC0->COUNT16.CTRLA.reg=TC_CTRLA_MODE_COUNT16|//8bitモード 
                          TC_CTRLA_PRESCALER(presc_val)|// 分周比を設定 
                          TC_CTRLA_PRESCSYNC_GCLK;// クロックはGCLKで同期化し
  TC0->COUNT16.WAVE.reg=TC_WAVE_WAVEGEN_MFRQ;//MFRQモードに設定 
    while(TC0->COUNT16.SYNCBUSY.reg);//同期を待つ
  Serial.println("tc_SYNCBUSY_wai2t_end");
    TC0->COUNT16.CC[CC_ID].reg=cc0;//カウンタ値を設定
    Serial.printf("EN=%u SB.EN=%u SB.CC0=%u\n", TC0->COUNT16.CTRLA.bit.ENABLE, TC0->COUNT16.SYNCBUSY.bit.ENABLE, TC0->COUNT16.SYNCBUSY.bit.CC0);
    while(TC0->COUNT16.SYNCBUSY.bit.CC0);//同期を待つ
      Serial.println("tc_SYNCBUSY_wait3_end");
      TC0->COUNT16.EVCTRL.reg |= TC_EVCTRL_MCEO0;
    TC0->COUNT16.CTRLA.bit.ENABLE=true;//TCを有効化
    while(TC0->COUNT16.SYNCBUSY.bit.ENABLE);//同期を待つ
    Serial.println("tc_config_mfrq_end");
    return TC0->COUNT16.CTRLA.bit.ENABLE;//成功した場合trueを返す
}
bool tc_enable_mceo()
{
    if(!TC0->COUNT16.EVCTRL.bit.MCEO0){
        return false;//MCEO0が有効化されなかった場合falseを返す
    }
    return true;//成功した場合trueを返す
}
void setup_tc(){
  //TCの初期化
  Serial.println("setup_tc_start");
  if(!tc_config_mfrq(PRESCALER_VAL,CC_0)){
    Serial.println("tc_config_mfrq_error");
  }
  if(!tc_enable_mceo()){
    Serial.println("tc_enable_mceo_error");
  }
    Serial.println("setup_tc_end");

    //確認
    Serial.printf("GENCTRL[%d]=0x%08lX\n", GCLK_ID, GCLK->GENCTRL[GCLK_ID].reg);  // GCLK3
    Serial.printf("PCHCTRL[%d]=0x%08lX\n", TC0_GCLK_ID, GCLK->PCHCTRL[TC0_GCLK_ID].reg);
}