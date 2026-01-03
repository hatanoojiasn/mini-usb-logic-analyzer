#pragma once
#define CLOCK_DIV 3//分周比を設定
#define TC_ID 0//使用するTCのIDを設定
#define GCLK_ID 3//使用するGCLKのIDを設定
#define GCLK_PCHCTRL_TC_ID TC0_GCLK_ID//GCLK_PCHCTRLで使用するTCのIDを設定
bool clock_init_gclk_dfll(int div);// DFLLを有効化し、GCLKに出力する
bool clock_attach_tc_gclk(int pchctrl_tc_id, int gclk_id);// TCにGCLKを割り当てる
void setup_clock();
