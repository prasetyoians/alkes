#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
 
#include "Wire.h"
#include "Adafruit_GFX.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>


Adafruit_MPU6050 mpu;

#define REPORTING_PERIOD_MS 1000
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
 
PulseOximeter pox;
 
uint32_t tsLastReport = 0;
 
const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 
void onBeatDetected()
{
Serial.println("Beat!");
}
 
void setup()
{
Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
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
 
if (!pox.begin()) {
Serial.println("FAILED");
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.println("FAILED");
display.display();
for(;;);
} else {
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.println("SUCCESS");
display.display();
Serial.println("SUCCESS");
}
pox.setOnBeatDetectedCallback(onBeatDetected);
}
 
void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  
pox.update();
 
if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
Serial.print("Heart BPM:");
Serial.print(pox.getHeartRate());
Serial.print("-----");
Serial.print("Oxygen Percent:");
Serial.print(pox.getSpO2());
Serial.println("\n");
Serial.print("-----");
Serial.print("Akselerasi - X:");
Serial.print(a.acceleration.x, 1);
Serial.print(",y:");
Serial.print(a.acceleration.y, 1);
Serial.print(",z:");
Serial.println(a.acceleration.z, 1);


 
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.print("Heart rate: ");
display.print(pox.getHeartRate());
display.print("bpm, SpO2: ");
display.print(pox.getSpO2());
display.print("%");
 

display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 16);
display.print("Akselo: x:");
display.print(a.acceleration.x, 1);
display.print(",y:");
display.print(a.acceleration.y, 1);
display.print(",z:");
display.println(a.acceleration.z, 1);




display.display();
tsLastReport = millis();
}
}
