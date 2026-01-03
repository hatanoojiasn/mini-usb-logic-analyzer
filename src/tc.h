#pragma once
#include "samd.h"
#include<Arduino.h>
#define PRESCALER_VAL  TC_CTRLA_PRESCALER_DIV4_Val//分周比を設定
#define CC_ID 0//CCの比較チャンネルのIDを設定
#define CC_0 50000//TCのカウンタ値を設定
void setup_tc();
bool tc_config_mfrq(int presc_val,int cc0);//分周比とカウンタ値を設定 成功したかを返す 
bool tc_enable_mceo();