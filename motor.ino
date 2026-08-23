#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ----- Motor A -----
int enA = 9;
int in1 = 8;
int in2 = 7;

// ----- Motor B -----
int enB = 3;
int in3 = 5;
int in4 = 4;

// ---- Speed Limits ----
float minA = 60, maxA = 100, speedA = minA, stepA = 0.25;
float minB = 40, maxB = 80,  speedB = minB, stepB = 0.25;

// ---- Hall Sensor ----
int hallPin = 2;       
volatile int pulseCount = 0;

unsigned long lastTime = 0;
float rps = 0;

// ---- Hall Interrupt ----
void hallISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  // ---- OLED Init ----
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Motor pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  // Hall pin
  pinMode(hallPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), hallISR, FALLING);

  // Motor direction
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void loop() {

  // ---- Smooth Motor A ----
  analogWrite(enA, (int)speedA);
  speedA += stepA;
  if (speedA >= maxA || speedA <= minA) stepA = -stepA;

  // ---- Smooth Motor B ----
  analogWrite(enB, (int)speedB);
  speedB += stepB;
  if (speedB >= maxB || speedB <= minB) stepB = -stepB;

  // ---- RPS Calculation ----
  if (millis() - lastTime >= 1000) {
    noInterrupts();
    int count = pulseCount;
    pulseCount = 0;
    interrupts();

    rps = count;

    Serial.print("RPS: ");
    Serial.println(rps);

    // ---- OLED Display ----
    display.clearDisplay();
    display.setCursor(10, 20);
    display.print("RPS: ");
    display.print(rps);
    display.display();

    lastTime = millis();
  }

  delay(30);
}