/*
 *  Arduino Pickup Winder RPM and Counter
 *  Arduino Nano
 *  Opto Interupter
 *  TM1637
 */

#include <TM1637Display.h>

// Digital connection pins
#define CLK 6
#define DIO 7

#define CLK1 8
#define DIO1 9

TM1637Display display(CLK, DIO);
TM1637Display display1(CLK1, DIO1);

unsigned long list[4] = {0, 0, 0, 0};
unsigned long currentMs;

volatile int counter = 1;
int length = 2;


void setup() {
  display.setBrightness(0x0A);
  display1.setBrightness(0x0A);
  attachInterrupt(0, count, RISING);
}


unsigned long *scroll (unsigned long *list, unsigned long n, int index) {
  list[index % 4] = n;
  return list;
}


int intervalFunc (unsigned long *list, int length, int index) {
  float interval = 0;
  for (int i = 0; i < length - 1; ++i) {
    interval += list[(index - i) % 4] - list[(index - i - 1) % 4];
  }

  return ((1000 / (interval / (length - 1)) * 60));
}


void count() {
  scroll(list, currentMs, counter);

  int intptr = intervalFunc(list, length, counter);
  display.showNumberDec(intptr);

  ++counter;
  if (length < 4) {
    ++length;
  }
}


void loop() {
  currentMs = millis();
  if (currentMs - list[(counter - 1) % 4] > 1000) {
    display.showNumberDec(0);
  }
  display1.showNumberDec(counter - 1);
}
