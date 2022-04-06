/*
 *  Arduino Pickup Winder Counter
 *  Arduino Nano
 *  Opto Interupter
 *  TM1637
 */

#include <TM1637Display.h>

// Digital connection pins
#define CLK 6
#define DIO 7

TM1637Display display(CLK, DIO);

volatile int counter = 0;


void setup() {
  display.setBrightness(0x0A);
  attachInterrupt(0, count, RISING);
}


void loop() {
  display.showNumberDec(counter);
  delay(100);
}


void count() {
  counter++;
}
