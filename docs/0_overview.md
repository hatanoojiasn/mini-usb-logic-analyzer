# 0. Overview

## プロジェクト概要
机上サイズの組込み作品「Mini USB Logic Analyzer」
- 開発期間：2025-06〜2026-01
- フェーズ構成：C-Series（MVP）→D-Series（Bootloader）

## 現フェーズ
- フェーズ：C-1 DMA
- 目的：DMAによる1ch@100kHz以上のサンプリング動確
- 成果物：clock/tc/evsys/dmaモジュール + 動確ログ

## 成果物と動確目標
| 区分 | 成果物 | 受入れ条件 |
|------|--------|------------|
| コード | DMAサンプリングAPI | GPIO入力変化をbufに格納できる |
| ドキュメント | 設計書(mdBook) | 構成図・API定義・試験手順を含む |

## 開発環境
- Target: Wio Terminal (ATSAMD51P19)
- Toolchain: arm-none-eabi-gcc 12 / VSCode + PlatformIO
- CI: GitHub Actions (build + cppcheck)