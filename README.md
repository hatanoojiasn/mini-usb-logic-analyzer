# mini-usb-logic-analyzer
【Embedded Project – Master Template】

==================================================
◆ A. プロジェクト全体概要　※固定
==================================================
目標        : 机上サイズで完結する組込み作品を 2 段構えで開発し、  
              担当者レベルの設計力・実装力・品質プロセスを証明する  
段階構成    : 1️⃣ C-Series   Mini USB Logic Analyzer (MVP v0.1)  
              2️⃣ D-Series   Secure OTA Bootloader  (v1.0)  
総期間      : 7 か月（2025-06 ～ 2026-01）  
学習時間    : 週 15–20 h  
ターゲットHW: Wio Terminal (ATSAMD51P19) + 最小周辺  
==================================================
◆ B. フェーズ別ロードマップ　※固定
==================================================
月 | フェーズ   | 主要成果物
-- | ---------- | ------------------------------
0  | Kick-off   | GitHub repo / CI skeleton
1  | C-1 DMA    | 1 ch @12 MHz capture, 帯域試算
1-2| C-2 USB    | USB Bulk stack, `capture.py`
2  | C-3 GUI    | 6 ch×24 MHz + PyQtGraph viewer
3  | C-4 Decode | I²C/SPI decoder, HW trigger
4  | C-5 MVP    | v0.1 release, 動画 & blog
5-6| D-1 Model  | Threat model, Flash map
6  | D-2 Port   | MCUboot on Wio, sign script
6-7| D-3 OTA    | BLE/Wi-Fi uploader, CRC32
7  | D-4 Safe   | Power-loss test, CI auto-OTA
7  | Wrap-up    | v1.0 release PDF / Portfolio
==================================================
◆ C. 開発環境・ルール　※固定
==================================================
Toolchain   : arm-none-eabi-gcc 12 / Rust 1.78  
Debug       : J-Link EDU Mini + OpenOCD  
CI          : GitHub Actions – gcc build, cppcheck, MISRA, cargo-clippy  
Coding Std. : MISRA-C 2012, rustfmt + clippy Pedantic  
Doc         : Doxygen (C), mdBook (Rust), Mermaid 図  
Repo URL    : https://github.com/hatanoojiasn/mini-usb-logic-analyzer.git
==================================================