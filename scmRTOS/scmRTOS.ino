#include <scmRTOS.h>

volatile uint8_t v;

void setup(void)
{
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    scmRTOS_START();
}

void loop(void)
{
    OS::sleep(100);
    digitalWrite(2, HIGH);
    OS::sleep(100);
    digitalWrite(2, LOW);
}

void loop_low(void)
{
    OS::sleep(100);
    digitalWrite(3, HIGH);
    OS::sleep(100);
    digitalWrite(3, LOW);
}

void loop_high(void)
{
    OS::sleep(100);
    digitalWrite(4, HIGH);
    OS::sleep(100);
    digitalWrite(4, LOW);
}

scmRTOS_PROCESS(0, 100, loop_high);
scmRTOS_PROCESS(1, 256, loop);
scmRTOS_PROCESS(2, 100, loop_low);
