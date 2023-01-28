#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino_FreeRTOS.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 5);

int side = 300;
int sunrise = 8;
int sunset = 17;
int TEST_PIN = 0;

int luxes;
int sensorReading;
int ledLevel;
const int ledCount = 10;

const int ledPins[] = {3, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const int LDR_PIN_DIG = 4;
const int METER_PIN = A0;


void setup () {
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.flush();
    abort();
  }


  pinMode(LDR_PIN_DIG, INPUT);

  lcd.init();
  lcd.backlight();

  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }

  xTaskCreate(Loading, "Task1", 110, NULL, 1, NULL);
  xTaskCreate(RTC, "Task2", 120, NULL, 2, NULL);
  xTaskCreate(Light, "Task3", 130, NULL, 3, NULL);
  xTaskCreate(Test, "Task4", 140, NULL, 4, NULL);
}

void loop () {
}

void Loading(void* pvParameters)
{ 
  while(1){
    sensorReading = analogRead(METER_PIN);
    ledLevel = map(sensorReading, 0, 1023, 0, ledCount);

    for (int thisLed = 0; thisLed < ledCount; thisLed++) {
      if (thisLed < ledLevel) {
        if (luxes >= side){
          digitalWrite(ledPins[thisLed], HIGH);
        }
        else {
          digitalWrite(ledPins[thisLed], LOW);
        }
      }
      else {
          digitalWrite(ledPins[thisLed], LOW);
      }
    }
    
    if (ledLevel >= 0 and ledLevel <= 9) {
      lcd.setCursor(15, 2);
      lcd.print(" ");
      lcd.setCursor(16, 2);
      lcd.print(ledLevel);
      lcd.setCursor(12, 3);
      lcd.print("        ");      
    }
    if (ledLevel == 10) {
      lcd.setCursor(15, 2);
      lcd.print(ledLevel);
      lcd.setCursor(12, 3);
      lcd.print("OVERVOLT");
    }
      
    lcd.setCursor(17, 2);
    lcd.print("/");
    lcd.setCursor(18, 2);
    lcd.print(ledCount);

    vTaskDelay(110/portTICK_PERIOD_MS);
  }
}

void Light(void* pvParameters)
{ 
  while(1){
    lcd.setCursor(0, 0);
    lcd.print("Outside ");
    luxes = analogRead(METER_PIN);

    if (luxes >= side)
    {
      if (luxes >= side) {
        digitalWrite(LDR_PIN_DIG, HIGH);
        lcd.setCursor(0, 3);
        lcd.print("Light");
      } 
    }

    if (luxes < side) {
      if (luxes < side) {
        digitalWrite(LDR_PIN_DIG, LOW);
        lcd.setCursor(0, 3);
        lcd.print("Dark");
        lcd.setCursor(4, 3);
        lcd.print(" ");
      }
    }

    if (luxes >= 0 and luxes < 10) {
      lcd.setCursor(0, 2);
      lcd.print(luxes);
      lcd.setCursor(1, 2);
      lcd.print("   ");
    }

    else if (luxes >= 10 and luxes < 100) {
      lcd.setCursor(0, 2);
      lcd.print(luxes);
      lcd.setCursor(2, 2);
      lcd.print("  ");
    }

    else if (luxes >= 100 and luxes < 1000) {
      lcd.setCursor(0, 2);
      lcd.print(luxes);
      lcd.setCursor(3, 2);
      lcd.print(" ");
    }

    else if (luxes >= 1000) {
      lcd.setCursor(0, 2);
      lcd.print(luxes);
    }

    lcd.setCursor(5, 2);
    lcd.print("lux");
    
    vTaskDelay(130/portTICK_PERIOD_MS);
  }
}

void RTC(void* pvParameters)
{
  while(1)
  {
    DateTime now = rtc.now();
    lcd.setCursor(0, 1);
    lcd.print(now.day());
    lcd.setCursor(2, 1);
    lcd.print(".");
    lcd.setCursor(3, 1);
    lcd.print(now.month());
    lcd.setCursor(5, 1);
    lcd.print(".");
    lcd.setCursor(6, 1);
    lcd.print(now.year());

    if (now.hour() < 10 and now.hour() >= 0) {
      lcd.setCursor(12, 1);
      lcd.print("0");
      lcd.setCursor(13, 1);
      lcd.print(now.hour());
    }

    else if (now.hour() >= 10 and now.hour() < 24) {
      lcd.setCursor(12, 1);
      lcd.print(now.hour());
    }

    lcd.setCursor(14, 1);
    lcd.print(":");
    
    if (now.minute() <= 10 and now.minute() >= 0) {
      lcd.setCursor(15, 1);
      lcd.print("0");
      lcd.setCursor(16, 1);
      lcd.print(now.minute());
    }

    else if (now.minute() > 10 and now.minute() <= 59) {
      lcd.setCursor(15, 1);
      lcd.print(now.minute());
    }

    lcd.setCursor(17, 1);
    lcd.print(":");

    if (now.second() <= 59 and now.second() >= 10) {
      lcd.setCursor(18, 1);
      lcd.print(now.second());
    }

    else if (now.second() <= 9 and now.second() >= 0) {
      lcd.setCursor(18, 1);
      lcd.print(" ");
      lcd.setCursor(19, 1);
      lcd.print(now.second());
    }

    if (now.hour() < sunset and now.hour() > sunrise) {
      lcd.setCursor(15, 0);
      lcd.print("Day");
    }    

    else {
      lcd.setCursor(15, 0);
      lcd.print("Night");
    }

  vTaskDelay(120/portTICK_PERIOD_MS);
  }
}

void Test(void* pvParameters)
{ 
  while(1){
  digitalWrite(TEST_PIN, HIGH);
  vTaskDelay(140/portTICK_PERIOD_MS);
  }
}
