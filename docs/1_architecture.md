# 1. Architecture
## 機能構成
この構成では、タイマTC0が一定周期でイベントを発生させ、
EVSYS経由でDMAをトリガし、GPIO入力値をメモリ上に順次サンプリングする。
CPUは介在せず、DMAが自動でデータ転送を行う
12 MHz動作を狙うため、CC0=4 で割り算され実質12 MHzサンプリング周期となる）
|必要な機能 | 呼び出しモジュール|
|------|------------------|　　　
| クロック源（DFLL48Mを分周比3で使用） | GCLKモジュール |
| GCLKをもとに任意の間隔でイベントを出力 | TCモジュール |
| TC0の比較一致イベントをEVSYS経由で転送 | EVSYSモジュール |
| DMAでGPIOレジスタからメモリへデータ転送 | DMAモジュール |
| 転送ログ出力（動作確認） | Logging（Serial出力） |

## クロック構成
```mermaid
flowchart LR
  A[DFLL48M] --> G3[GCLK3 / DIV=3]
  G3 -->|PCHCTRL(TC0)| T0[TC0]
  T0 --> EV[EVSYS: GEN=TC0_MCX0 → CH0 → USER=DMAC_CH0]
  EV --> D0[DMAC CH0]

