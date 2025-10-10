#include <Arduino.h>
#include "clock.h"
#include "dma.h"
void setup()
{
    Serial.begin(115200);
    while(!Serial);
    Serial.println("DMA Test1");
    setup_clock();
    setup_dma();
       Serial.println("SetupEnd");
}
void loop()
{
    Serial.println("Loop Start");
    static int count =0;
     count ++;
        
            PORT->Group[0].OUT.reg = count;
            Serial.printf("num = %d\n",&buf[0]);
        if(count >= 10000)
        {
            PORT->Group[0].OUT.reg = 0;
            count = 0;
        }
        delay(1000); // Wait for a second to observe the output
}
