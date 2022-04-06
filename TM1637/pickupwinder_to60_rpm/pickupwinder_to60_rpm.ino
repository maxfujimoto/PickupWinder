/*
 *  Arduino Pickup Winder RPM
 *  Arduino Nano
 *  Opto Interupter
 *  TM1637
 */

#include <TM1637Display.h>


#define CLK 6
#define DIO 7

TM1637Display display(CLK, DIO);

volatile int counter = 0;


void setup() {
  display.setBrightness(0x0A);
  attachInterrupt(0, count, RISING);
}


void loop() {
  display.showNumberDec(counter * 60);
  counter = 0;
  delay(1000);
}


void count() {
  counter++;
}
