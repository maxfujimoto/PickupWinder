/*
 *  RPM, Counter and Gauss Meter
 *  Arduino Nano
 *  SS495A Hall Effect Sensor
 *  16x2 I2C LCD
 *  2 x TM1637
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h>

// TM1637 digital connection pins
#define CLK 6
#define DIO 7

#define CLK1 8
#define DIO1 9

TM1637Display display(CLK, DIO);
TM1637Display display1(CLK1, DIO1);

LiquidCrystal_I2C lcd(0x27, 16, 2);

static int voltageZero = 0,
           peak = 0;

int voltage = 0,
    gauss = 0,
    gaussAbs = 0;

unsigned long list[4] = {0, 0, 0, 0};
unsigned long currentMs;

volatile int counter = 1;
int length = 2;


void setup() {
  lcd.init();
  lcd.backlight();

  display.setBrightness(0x0A);
  display1.setBrightness(0x0A);

  attachInterrupt(0, count, RISING);
  voltageZero = analogRead(A0);

  // Set cursor column, row
  //Static Text
  lcd.setCursor(0, 0);
  lcd.print("Volts");

  lcd.setCursor(6, 0);
  lcd.print("Gauss");

  lcd.setCursor(12, 0);
  lcd.print("Peak");
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

  gaussValues();
  gaussDisplay();
}


void gaussValues() {
  voltage = analogRead(A0);
  gauss = (voltage - voltageZero) * 2.57;
  gaussAbs = abs(gauss);
}


int peakFunc () {
  static int threshold = 0;

  if (gaussAbs > threshold && gaussAbs > 8) {
    threshold = gaussAbs;
    peak = gauss;
  }
  else if (gaussAbs < 8) {
    threshold = 0;
  }

    return peak;
}


char polarity (int v, int vZero) {
  if (v < vZero - 3) {
    return ('S');
  }
  else if (v > vZero + 3) {
    return ('N');
  }
  else {
    return (' ');
  }
}


// Add padding spaces to overwrite previous digits, absolute and reduce noise around 0.
void padding(int var, int x, int y) {
  var = abs(var);

  lcd.setCursor(x, y);

  if (var < 10) {
    lcd.print("   ");
    lcd.setCursor(x + 3, y);
  }
  else if (var < 100) {
    lcd.print("  ");
    lcd.setCursor(x + 2, y);
  }
  else if (var < 1000) {
    lcd.print(" ");
    lcd.setCursor(x + 1, y);
  }

  if (var < 6) {
    lcd.print("0");
  } else if (var > 9999) {
    lcd.print("eror");
  } else {
    lcd.print(var);
  }
}


void gaussDisplay() {
  // Set cursor column, row
  padding(voltage, -1, 1);


  padding(gaussAbs, 6, 1);

  lcd.setCursor(5, 1);
  lcd.print(polarity(voltage, voltageZero));


  lcd.setCursor(11, 1);
  lcd.print(polarity(peak, 0));

  padding (peakFunc(), 12, 1);
}
