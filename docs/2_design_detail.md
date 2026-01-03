# 2. Design Detail

## Mainモジュール
### 関数一覧
| 関数名 | 概要 | 引数 | 戻り値 |
|---------|------|------|--------|
| setup()| シリアル、クロック、TC、EVESYS、DMAの設定及び動作を開始させる | - | - |
| loop()|シリアルによりDMAで読み込まれた値を一定周期で確認| - | - |

### 設計意図
- DFLL48Mを基準にGCLK3で約16MHz生成。  
- 同期待ち時間を50msとし、タイムアウト時にERRログを出力。

---
## Clockモジュール
### 関数一覧
| 関数名 | 概要 | 引数 | 戻り値 |
|---------|------|------|--------|
| clock_init_gclk3_dfll_div(div) | DFLLからGCLK3を生成する | div: 分周値 | bool: 成功/失敗 |
| clock_attach_tc0_gclk3() | TC0にGCLK3を割り当てる | なし | bool |

### 設計意図
- DFLL48Mを基準にGCLK3で約16MHz生成。  
- 同期待ち時間を50msとし、タイムアウト時にERRログを出力。

---

## TCモジュール
### 関数一覧
| 関数名 | 概要 | 引数 | 戻り値 |
|---------|------|------|--------|
| tc0_config_mfrq(presc, cc0) | MFRQモード設定 | presc: 分周設定 / cc0: 比較値 | bool |
| tc0_enable_mceo0(en) | MCEO0出力を有効化 | en: true/false | bool |

### 設計意図
- 比較一致ごとにEVSYSへイベントを送る。
- CC0初期値1000（安全確認用）。後に12MHz用に再調整。

---

## DMAモジュール
### 関数一覧
| 関数名 | 概要 | 引数 | 戻り値 |
|---------|------|------|--------|
| dma_global_enable() | DMAC全体を有効化 | なし | void |
| dma_mem2mem_once(src, dst, words) | メモリ間転送を1回実施 | ソース/デスティネーション/データ| bool |
| dma_gpio_sample_once(reg, dst, words) | GPIOレジスタからサンプリング転送 | ソース/デスティネーション/データ | bool |
| dma_ch0_wait_done(timeout_ms) | 転送完了待ち | タイムアウト時間 | bool |

### 設計意図
- DMAC_CH0でEVSYSトリガにより転送開始。  
- 転送中はCPU待機、完了後フラグをクリア。  
- `BTCTRL_BLOCKACT_INT`使用→完了時に割込みフラグ確認。

---

## EVSYSモジュール
- GEN: TC0_MCX0  
- CH: 0  
- USER: DMAC_CH0  
- DMACのトリガはEVSYS CH0経由で受信。

---

## Logging
- Serial出力を共通関数化予定。  
- タグ付きログ形式「TAG: message」。