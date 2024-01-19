#include <LiquidCrystal.h>

const byte rs = 7;
const byte en = 8;
const byte d4 = 10;
const byte d5 = 11;
const byte d6 = 12;
const byte d7 = 13;

const int trigPinFront = 4;
const int trigPinBack = 9;
const int echoPinFront = 5;
const int echoPinBack = 6;
int distanceBack = 0;
int distanceFront = 0;

const int ledPin = 2;
byte ledState = LOW;

const int buzzerPin = 3;

const String clear = "       ";

const short readingDelay = 250;
unsigned long lastReadingTime = 0;
unsigned long previousMillis = 0;

const short slowDelay = 500;
const short mediumDelay = 250;
const short fastDelay = 100;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a message to the LCD.
  lcd.print("Front: ");

  lcd.setCursor(0, 1);
  lcd.print("Back: ");

  pinMode(trigPinFront, OUTPUT); // Sets the trigPin as an Output
  pinMode(trigPinBack, OUTPUT);
  pinMode(echoPinBack, INPUT); // Sets the echoPin as an Input
  pinMode(echoPinFront, INPUT);

  pinMode(ledPin, OUTPUT);

  Serial.begin(9600); // Starts the serial communication
}

int digitCount(int a)
{
  int cnt = 0;
  while (a) {
    cnt++;
    a/=10;
  }
}

void DistanceSensor(int echoPin, byte postion, int distance)
{
  Serial.print(postion);
  Serial.print(" ");
  Serial.println(distance);

  lcd.setCursor(8, postion);
  lcd.print(clear);
  if (distance < 1000) {
    lcd.setCursor(8 + digitCount(distance), postion);
    lcd.print(distance);
  }
}

void handleSensors()
{
  if (millis() - lastReadingTime > readingDelay) {
    lastReadingTime = millis();

    digitalWrite(trigPinFront, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPinFront, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinFront, LOW);

    long durationFront = pulseIn(echoPinFront, HIGH);
    distanceFront = durationFront*0.034/2;

    DistanceSensor(echoPinFront, 0, distanceFront);

    digitalWrite(trigPinBack, LOW);
    delayMicroseconds(2);
    
    digitalWrite(trigPinBack, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinBack, LOW);

    long durationBack = pulseIn(echoPinBack, HIGH);
    distanceBack = durationBack*0.034/2;
 
    DistanceSensor(echoPinBack, 1, distanceBack);
  }
}

void updateLED(const short delay)
{
  if (millis() - previousMillis >= delay) {
      previousMillis = millis();
      if (ledState == LOW) {
        tone(buzzerPin, 600, delay / 1.5);
        ledState = HIGH;
      } else {
        noTone(buzzerPin);
        ledState = LOW;
      }
      digitalWrite(ledPin, ledState);
    }
}

void handleLED()
{
  if (distanceBack < 10 || distanceFront < 10) {
    updateLED(fastDelay);
  } else if (distanceBack < 25 || distanceFront < 25) {
    updateLED(mediumDelay);
  } else if (distanceBack < 50 || distanceFront < 50) {
    updateLED(slowDelay);
  } else {
    noTone(buzzerPin);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}

void loop() {
  handleSensors();
  handleLED();
}