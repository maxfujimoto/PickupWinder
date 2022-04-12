/*
 *  Arduino Pickup Winder RPM
 *  Arduino Nano
 *  Opto Interupter
 *  TM1637
 */

#include <TM1637Display.h>

// Digital connection pins
#define CLK 6
#define DIO 7

TM1637Display display(CLK, DIO);

unsigned long list[2] = {0, 0};
unsigned long currentMs;

bool index = 0;


void setup() {
  display.setBrightness(0x0A);
  attachInterrupt(0, count, RISING);
}


int intervalFunc (unsigned long *list, int index) {
  float interval = 0;
  interval = list[index] - list[!index];
  return ((1000 / interval) * 60);
}

void count() {
  list[index] = currentMs;
  
  int intptr = intervalFunc(list, index);
  display.showNumberDec(intptr);

  index = !index;
}


void loop() {
  currentMs = millis();
  if (currentMs - list[!index] > 1000) {
    display.showNumberDec(0);
  }
}
