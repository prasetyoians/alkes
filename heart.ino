#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include "Wire.h"
#include "Adafruit_GFX.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>


int plus;
Adafruit_MPU6050 mpu;

#define BUTTON_PIN 21  // GIOP21 pin connected to button

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


#define REPORTING_PERIOD_MS 100
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

PulseOximeter pox;

uint32_t tsLastReport = 0;

const unsigned char bitmap[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
  0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
  0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
  0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
  0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
  0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
  0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU6050 sensor");


  display.display();

  display.begin();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(0, 0);

  display.println("Initializing pulse oximeter..");
  display.display();
  Serial.print("Initializing pulse oximeter..");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

String data = "";

void loop() {
  if (Serial.available() > 0) {
    data = Serial.readString();
    data.trim();
    while (!pox.begin()) {
      Serial.println("FAILED");
    }
  }
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(300);
    data = "b";
    while (!pox.begin()) {
      Serial.println("FAILED");
    }
  }
  if (data.equals("b")) {
    display.clearDisplay();
    Serial.println("Counting...");
    display.setTextSize(1);
    display.setTextColor(1);
    display.setCursor(0, 0);
    display.println("Counting...");
    display.display();
    data = "";

    int oxy = 0, countOxy = 0;
    int heart = 0;

    // int posX = 0, posY = 0, posZ = 0;
    //

    // baca pox
    for (int x = 0; x < REPORTING_PERIOD_MS; x++) {
      pox.update();

      int aa = pox.getSpO2();
      int bb = pox.getHeartRate();
      if (aa > 0 && bb > 0) {
        countOxy++;
        oxy = oxy + aa;
        heart = heart + bb;
        Serial.print(".");
      }
      delay(REPORTING_PERIOD_MS / 2);
    }

    display.clearDisplay();
    Serial.println("");
    Serial.print("hr: ");
    if (heart > 0) {
      Serial.print(heart / countOxy);
    } else {
      Serial.print("");
    }
    Serial.println("BPM");

    Serial.print("oxy: ");
    if (oxy > 0) {
      Serial.print(oxy / countOxy);
    } else {
      Serial.print("");
    }
    Serial.println("%");

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(1);
    display.setCursor(0, 0);

    display.println("");
    display.print("hr: ");
    if (heart > 0) {
      Serial.print(heart / countOxy);
    } else {
      Serial.print("");
    }
    display.println("BPM");

    display.print("oxy: ");
    if (oxy > 0) {
      Serial.print(oxy / countOxy);
    } else {
      Serial.print("");
    }
    display.println("%");
    display.display();

    // mpu
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    display.setCursor(0, 16);
    Serial.print("Akselo: x:");
    Serial.print(a.acceleration.x, 1);
    Serial.print(",y:");
    Serial.print(a.acceleration.y, 1);
    Serial.print(",z:");
    Serial.println(a.acceleration.z, 1);
  }
}
