#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char ZERO_CHAR = '0';

const char STATE_ZERO_OFF = 0; // currently Off, push count 0
const char STATE_ONE_ON = 1; // currently On, push count 1
const char STATE_ONE_OFF = 2; // currently Off, push count 1
const char STATE_TWO_ON = 3;  // currently On, push count 2
const char STATE_TWO_OFF = 4;  // currently Off, push count 2
const char STATE_THREE_ON = 5;  // currently On, push count 3
const char STATE_THREE_OFF = 6;  // currently Off, push count 3
const char STATE_ONE_LONG = 7; // currently holding, push count 1
const char STATE_TWO_LONG = 8; // currently holding, push count 2
const char STATE_THREE_LONG = 9; // currently holding, push count 3 (not used)

const int keyPin = 7;
const int STATE_SWITCH_THRESHOLD = 300;

unsigned long base = 0;
unsigned long lastEventTime = 0;
boolean lastKey = HIGH;
boolean currentKey = HIGH;
char currentState = STATE_ZERO_OFF;
char memory[] = "00:00:00";

unsigned char hour, minute, second;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void updateMemory(unsigned long diff) {
  // make sure using unsigned long; 60 * 60 * 1000 would overflow)
  hour = diff / (60UL * 60UL * 1000UL) % 24;
  minute = diff / (60UL * 1000UL) % 60;
  second = diff / 1000UL % 60;
  memory[0] = ZERO_CHAR + hour / 10;
  memory[1] = ZERO_CHAR + hour % 10;
  memory[3] = ZERO_CHAR + minute / 10;
  memory[4] = ZERO_CHAR + minute % 10;
  memory[6] = ZERO_CHAR + second / 10;
  memory[7] = ZERO_CHAR + second % 10;
}

void ioChangeProcess(unsigned long nowTime, char state) {
  switch (currentState) {
    case STATE_ZERO_OFF:
      currentState = STATE_ONE_ON;
      lastEventTime = nowTime;
      break;
    case STATE_ONE_ON:
      currentState = STATE_ONE_OFF;
      lastEventTime = nowTime;
      break;
    case STATE_ONE_OFF:
      currentState = STATE_TWO_ON;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_ON:
      currentState = STATE_TWO_OFF;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_OFF:
      currentState = STATE_THREE_ON;
      lastEventTime = nowTime;
      break;
    case STATE_THREE_ON:
      currentState = STATE_THREE_OFF;
      lastEventTime = nowTime;
      break;
    case STATE_ONE_LONG:
    case STATE_TWO_LONG:
    case STATE_THREE_LONG:
      currentState = STATE_ZERO_OFF;
      lastEventTime = nowTime;
      break;
  }
}

void timeBasedProcess(unsigned long nowTime, char state) {
  switch (state) {
    case STATE_ONE_ON:
    case STATE_ONE_LONG:
      currentState = STATE_ONE_LONG;
      base -= 60UL * 60UL * 1000UL;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_ON:
    case STATE_TWO_LONG:
      currentState = STATE_TWO_LONG;
      base -= 5UL * 60UL * 1000UL;
      lastEventTime = nowTime;
      break;
    case STATE_THREE_ON:
      lastEventTime = nowTime;
      break;
    case STATE_ONE_OFF:
      currentState = STATE_ZERO_OFF;
      base -= 60UL * 60UL * 1000UL;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_OFF:
      currentState = STATE_ZERO_OFF;
      base -= 5UL * 60UL * 1000UL;
      lastEventTime = nowTime;
      break;
    case STATE_THREE_OFF:
      currentState = STATE_ZERO_OFF;
      base = millis();
      lastEventTime = nowTime;
      break;
  }
  return processed;
}

void setup()
{
  lcd.init();
  lcd.backlight();
}

void loop()
{
  unsigned long nowTime = millis();
  
  currentKey = digitalRead(keyPin);
  if (currentKey != lastKey) {
    ioChangeProcess(nowTime, state);
  } else if (nowTime - lastEventTime >= STATE_SWITCH_THRESHOLD || currentState == STATE_THREE_OFF) {
    timeBasedProcess(nowTime, currentState);
  }
  lastKey = currentKey;
  
  updateMemory(nowTime - base);
  lcd.setCursor(0, 0);
  lcd.print(memory); 
}


