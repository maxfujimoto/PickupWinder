/*
 *  RPM, Counter and Gauss Meter
 *  Arduino Nano
 *  SS495A Hall Effect Sensor
 *  Optointerupter
 *  16x2 I2C LCD
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Assign digital pin for switch.
#define SWITCH 7

LiquidCrystal_I2C lcd(0x27, 16, 2);

static int voltageZero = 0,
           peak = 0;

int voltage = 0,
    gauss = 0,
    gaussAbs = 0;

unsigned long list[4] = {0, 0, 0, 0};
unsigned long currentMs;

volatile int counter = 1;
byte length = 2;

int intptr = 0;

volatile bool switchState = 0,
              i = switchState;


void setup() {
  lcd.init();
  lcd.backlight();

  pinMode( SWITCH, INPUT_PULLUP );

  attachInterrupt(0, count, RISING);
  voltageZero = analogRead(A0);
}


unsigned long *scroll (unsigned long *list, unsigned long n, int index) {
  list[index % 4] = n;
  return list;
}


void intervalFunc (unsigned long *list, int length, int index) {
  float interval = 0;
  for (int i = 0; i < length - 1; ++i) {
    interval += list[(index - i) % 4] - list[(index - i - 1) % 4];
  }

  intptr = ((1000 / (interval / (length - 1)) * 60));
}


//Padding for 4 decimal numbers
void fourDecPadding(int var, int x, int y) {
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

  if (var > 9999) {
    lcd.print("eror");
  } else {
    lcd.print(var);
  }
}


//Wipes full screen
void wipe() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}


void count() {
  scroll(list, currentMs, counter);

  intervalFunc(list, length, counter);

  ++counter;
  if (length < 4) {
    ++length;
  }
}


void rpmCountDisplay() {
  lcd.setCursor(2, 0);
  lcd.print("COUNT");

  lcd.setCursor(10, 0);
  lcd.print("RPM");

  if (currentMs - list[(counter - 1) % 4] > 1000) {
    intptr = 0;
  }
  fourDecPadding(counter - 1, 2, 1);
  fourDecPadding(intptr, 10, 1);
}


void loop() {
  currentMs = millis();
  switchState = digitalRead(SWITCH);

  if (switchState == 1) {
    for (i; i != switchState; i = !i) {
      wipe();
    }
    rpmCountDisplay();
  } else {
    for (i; i != switchState; i = !i) {
      wipe();
    }
    gaussValues();
    gaussDisplay();
  }
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
    lcd.print("  ");
    lcd.setCursor(x + 2, y);
  }
  else if (var < 100) {
    lcd.print(" ");
    lcd.setCursor(x + 1, y);
  }

  if (var < 6) {
    lcd.print("0");
  }
  else if (var > 999) {
    lcd.print("err");
  } else {
    lcd.print(var);
  }
}


void gaussDisplay() {
  // Set cursor column, row
  lcd.setCursor(0, 0);
  lcd.print("Volts");

  padding(voltage, 0, 1);

  lcd.setCursor(6, 0);
  lcd.print("Gauss");

  padding(gaussAbs, 7, 1);

  lcd.setCursor(6, 1);
  lcd.print(polarity(voltage, voltageZero));

  lcd.setCursor(12, 0);
  lcd.print("Peak");

  lcd.setCursor(12, 1);
  lcd.print(polarity(peak, 0));

  padding(peakFunc(), 13, 1);
}