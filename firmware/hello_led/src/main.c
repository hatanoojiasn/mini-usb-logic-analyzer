#include <Arduino.h>
#define LED_BUILTIN 15
int main(void){
    pinMode(LED_BUILTIN, OUTPUT);
    while(1){
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    }
}