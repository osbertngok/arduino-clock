#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char ZERO_CHAR = '0';
const char STATE_ZERO_OFF = 0;
const char STATE_ONE_ON = 1;
const char STATE_ONE_OFF = 2;
const char STATE_TWO_ON = 3;
const char STATE_TWO_OFF = 4;
const char STATE_THREE_ON = 5;
const char STATE_THREE_OFF = 6;
const char STATE_ONE_LONG = 7;
const char STATE_TWO_LONG = 8;
const char STATE_THREE_LONG = 9;

const int keyPin = 7;

unsigned long base = 0;
unsigned long lastEventTime = 0;
boolean lastKey = HIGH;
boolean currentKey = HIGH;
char currentState = STATE_ZERO_OFF;
char memory[] = "00:00:00";

unsigned char hour, minute, second;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void updateMemory(unsigned long diff) {
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

boolean process(unsigned long nowTime, char state) {
  boolean processed = false;
  switch (state) {
    case STATE_ONE_ON:
    case STATE_ONE_LONG:
      currentState = STATE_ONE_LONG;
      base -= 60UL * 60UL * 1000UL;
      processed = true;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_ON:
    case STATE_TWO_LONG:
      currentState = STATE_TWO_LONG;
      base -= 5UL * 60UL * 1000UL;
      processed = true;
      lastEventTime = nowTime;
      break;
    case STATE_THREE_ON:
      processed = true;
      lastEventTime = nowTime;
      break;
    case STATE_ONE_OFF:
      currentState = STATE_ZERO_OFF;
      base -= 60UL * 60UL * 1000UL;
      processed = true;
      lastEventTime = nowTime;
      break;
    case STATE_TWO_OFF:
      currentState = STATE_ZERO_OFF;
      base -= 5UL * 60UL * 1000UL;
      processed = true;
      lastEventTime = nowTime;
      break;
    case STATE_THREE_OFF:
      currentState = STATE_ZERO_OFF;
      base = millis();
      processed = true;
      lastEventTime = nowTime;
      break;
  }
  return processed;
}

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop()
{
  Serial.print(currentState);
  unsigned long nowTime = millis();
  
  // Don't response for the first second
  if (nowTime > 3000) {
    currentKey = digitalRead(keyPin);
    if (currentKey != lastKey) {
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
    } else {
      if (nowTime - lastEventTime >= 300 || currentState == STATE_THREE_OFF) {
        process(nowTime, currentState);
      }
    }
    lastKey = currentKey;
  }
  
  updateMemory(nowTime - base);
  lcd.setCursor(0, 0);
  lcd.print(memory); 
}


