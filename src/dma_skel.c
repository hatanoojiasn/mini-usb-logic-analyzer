//DMAスケルトン
/*タイマ起動処理　void setup_timer()
{
タイマ設定内容
使用タイマ：　DFLL48M(48MHZ)
分周比　　：　4
出力先　　:　GCLK_TC0 (index9)
}
*/
/* DMA起動処理　void setup_dma()
{
DMA設定内容
使用チャンネル；０
トリガ内容：サンプル書き込み
トリガした際のアクション：送信
}
*/ 
/* 定期処理
void loop()
{
タイマタスク通知
}
*/
/*
タイマタスク　void timer_task()
{
RAM容量読み込み
}
*/