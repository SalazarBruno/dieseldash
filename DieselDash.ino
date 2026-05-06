/*
 *  DieselDash v1.1.2 - Professional Layout (Colon Separator)
 *  Copyright (C) 2024
 *  Distributed under the GNU General Public License v3.0
 */

#include <LiquidCrystal.h>

/* -----------------------------------------------------------
 *  1. CONFIGURATION & PINS
 * ----------------------------------------------------------- */
#define ENABLE_EGT
#define ENABLE_AFR

#ifdef ENABLE_EGT
  #define EGT_INNOVATE_TC4    // 0V=24C, 5V=1093C
#endif

#ifdef ENABLE_AFR
  #define AFR_INNOVATE_LC1    // 0V=7.35, 5V=22.39
#endif

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int alarmLED = 2;    
const int buzzer = 3;      
const int afrInputPin = A5;
const int buttonPin = A0;  

float peakTemps[] = {0, 0, 0, 0};
int egtAlarmLimit = 750;     
float richAlarmLimit = 16.5; 
int currentScreen = 0; 

bool showPeaks = false;
unsigned long lastBlink = 0;
unsigned long lastDisplayUpdate = 0;
const int displayInterval = 200; 
bool ledState = false;

/* -----------------------------------------------------------
 *  2. SCALING HELPERS
 * ----------------------------------------------------------- */
float scaleEGT(int raw) {
  #ifdef EGT_INNOVATE_TC4
    return (raw * (1093.0 - 24.0) / 1023.0) + 24.0;
  #endif
  return 0;
}

float scaleAFR(int raw) {
  #ifdef AFR_INNOVATE_LC1
    return (raw * (22.39 - 7.35) / 1023.0) + 7.35;
  #endif
  return 0;
}

/* -----------------------------------------------------------
 *  3. LOGIC FUNCTIONS
 * ----------------------------------------------------------- */
void checkSafetyLimits() {
  bool egtDanger = false;
  bool afrDanger = false;

  #ifdef ENABLE_EGT
  for (int i = 0; i < 4; i++) {
    int raw = analogRead(i + 1);
    if (raw > 5 && raw < 1018) {
      float currentC = scaleEGT(raw);
      if (currentC > peakTemps[i]) peakTemps[i] = currentC;
      if (currentC >= egtAlarmLimit) egtDanger = true;
    }
  }
  #endif

  #ifdef ENABLE_AFR
  int rawAFR = analogRead(afrInputPin);
  if (rawAFR > 5 && rawAFR < 1018) {
    if (scaleAFR(rawAFR) < richAlarmLimit) afrDanger = true;
  }
  #endif

  handleAlarms(egtDanger, afrDanger);
}

void handleAlarms(bool egt, bool afr) {
  if (egt) {
    if (millis() - lastBlink > 150) {
      lastBlink = millis(); ledState = !ledState;
      digitalWrite(alarmLED, ledState);
      if (ledState) tone(buzzer, 2500); else noTone(buzzer);
    }
  } else if (afr) {
    digitalWrite(alarmLED, HIGH);
    if (millis() - lastBlink > 400) {
      lastBlink = millis(); ledState = !ledState;
      if (ledState) tone(buzzer, 800); else noTone(buzzer);
    }
  } else {
    digitalWrite(alarmLED, LOW);
    noTone(buzzer);
  }
}

void resetPeaks() {
  for(int i=0; i<4; i++) peakTemps[i] = 0;
  lcd.clear();
  lcd.print("PEAKS RESET");
  delay(500);
  lcd.clear();
}

/* -----------------------------------------------------------
 *  4. BUTTON & DISPLAY HANDLERS
 * ----------------------------------------------------------- */
void handleButtons() {
  int btn = analogRead(buttonPin);
  static bool btnReleased = true;

  if (btn > 1000) { btnReleased = true; return; }
  if (!btnReleased) return; 

  if (btn < 100) { // RIGHT
    currentScreen = (currentScreen + 1) % 3;
    lcd.clear();
    btnReleased = false;
  }
  else if (btn < 450 && btn > 350) { // LEFT
    currentScreen--;
    if (currentScreen < 0) currentScreen = 2;
    lcd.clear();
    btnReleased = false;
  }
  else if (btn < 250 && btn > 150) showPeaks = true;  
  else if (btn < 650 && btn > 500) showPeaks = false; 
  else if (btn < 850 && btn > 700) resetPeaks();
}

void drawEGTs(int col1, int col2, bool detailed) {
  #ifdef ENABLE_EGT
  for (int i = 0; i < 4; i++) {
    int raw = analogRead(i + 1);
    int col = (i % 2 == 0) ? col1 : col2;
    int row = (i < 2) ? 0 : 1;
    lcd.setCursor(col, row);
    
    char label = 'a' + i;
    lcd.print(label);
    
    // Using the Colon separator as requested
    if (detailed) lcd.print(":"); 

    if (raw < 5 || raw > 1018) {
      lcd.print(detailed ? " ERR " : "ERR ");
    } else {
      int val = (int)(showPeaks ? peakTemps[i] : scaleEGT(raw));
      
      if (detailed) {
        // Professional Right-Alignment logic
        if (val < 1000) lcd.print(" ");
        if (val < 100) lcd.print(" ");
        lcd.print(val);
        lcd.print("\xDF"); // Degree symbol
      } else {
        lcd.print(val);
        lcd.print("  "); // Clear trailing space
      }
    }
  }
  #endif
}

void drawAFR(int labelCol, int valCol) {
  #ifdef ENABLE_AFR
  lcd.setCursor(labelCol, 0); lcd.print("AFR");
  lcd.setCursor(valCol, 1);
  float afr = scaleAFR(analogRead(afrInputPin));
  if (afr < 5 || afr > 1018) lcd.print("--- ");
  else {
    if (afr < 10.0) lcd.print(" ");
    lcd.print(afr, 1);
  }
  #endif
}

void updateLCD() {
  switch (currentScreen) {
    case 0: 
      drawEGTs(0, 6, false);
      drawAFR(13, 12);
      break;
    case 1: 
      drawEGTs(0, 8, true); 
      break;
    case 2: 
      lcd.setCursor(1, 0); lcd.print("AIR-FUEL RATIO");
      lcd.setCursor(6, 1);
      float afrVal = scaleAFR(analogRead(afrInputPin));
      lcd.print(afrVal, 1); lcd.print("  ");
      break;
  }
}

void setup() {
  lcd.begin(16, 2);
  pinMode(alarmLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.print("DIESELDASH v1.1");
  delay(1500);
  lcd.clear();
}

void loop() {
  checkSafetyLimits(); 
  handleButtons();
  if (millis() - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = millis();
    updateLCD();
  }
}

