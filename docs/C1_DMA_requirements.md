# DMAクロック(トリガー)源
DMAのトリガ源としてタイマ割り込みを設定する
タイマの設定は以下
使用タイマ:DFLL48M(48MHZ)
分周比:4
周波:12MHZ
出力先:GCLK_TC0 (index9)
# 電圧入力ピン
pb16
#リング長
4096
# TCレジスタ設定
GCLK_TC0.CTRLA.ENABLE = 1(クロック供給源)
GCLK_TC0.CTRLA.MODE =  0(16BITMODE)
GCLK_TC0.CTRLA.PRESCALER = 3(分周比8)

