#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include "Wire.h"
#include "Adafruit_GFX.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "solo";
const char* password = "123412345";


int plus;
Adafruit_MPU6050 mpu;

#define BUTTON_PIN 21  // GIOP21 pin connected to button

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


#define REPORTING_PERIOD_MS 200


int sensorDataHeart[REPORTING_PERIOD_MS];  // Array untuk menyimpan data
int sensorDataOxy[REPORTING_PERIOD_MS];  // Array untuk menyimpan data

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
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 

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

  int aa = 0 ;
 int bb =0;
    // baca pox    
    for (int x = 0; x < REPORTING_PERIOD_MS; x++) {
   pox.update();

       aa = pox.getSpO2();
      bb = pox.getHeartRate();
      
      if (aa > 0 && bb > 0) {
        countOxy++;
        oxy = oxy + aa;
        heart = heart + bb;
        Serial.print(".");
        sensorDataHeart[x] = bb;
        sensorDataOxy[x] = aa;

      }
      delay(REPORTING_PERIOD_MS / 2);
    }

  int dataSizeHeart = sizeof(sensorDataHeart) / sizeof(sensorDataHeart[0]);
  int dataSizeOxy = sizeof(sensorDataOxy) / sizeof(sensorDataOxy[0]);

int modeValueHeart = calculateMode(sensorDataHeart, dataSizeHeart);
int modeValueOxy = calculateMode(sensorDataOxy, dataSizeOxy);

  // Menampilkan hasil modus di Serial Monitor
 
  for (int i = 0; i < REPORTING_PERIOD_MS; i++) {
    Serial.print("Data ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(sensorDataHeart[i]);
  }
  
  Serial.print("Modus dari data heart adalah: ");
  Serial.println(modeValueHeart);

  

    display.clearDisplay();
    Serial.println("");
    Serial.print("hr: ");
    if (bb > 0) {
      Serial.print("hr: ");
      Serial.println(heart);
      Serial.print("modus: ");

      Serial.print(modeValueHeart);
    } else {
      Serial.print("");
    }
    Serial.println("BPM");

    Serial.print("oxy: ");
    if (aa > 0) {
      Serial.print(modeValueOxy);
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
    if (bb > 0) {
      display.print(modeValueHeart);
    } else {
      display.print("");
    }
    display.println("BPM");

    display.print("oxy: ");
    if (aa > 0) {
      display.print(modeValueOxy);
    } else {
      display.print("");
    }
    display.println("%");



    // mpu
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    display.setCursor(0, 16);
    display.print("Akselo: x:");
    display.print(a.acceleration.x, 1);
    display.print(",y:");
    display.print(a.acceleration.y, 1);
    display.print(",z:");
    display.println(a.acceleration.z, 1);
    display.display();
         String serverName2 = "http://127.0.0.1:3000/senddatatosps?hr="+String(modeValueHeart)+"&spo2="+String(modeValueOxy)+"&akselox="+String(a.acceleration.x)+"&akseloy="+String(a.acceleration.y)+"&akseloz="+String(a.acceleration.z);

        String sensorReadings = httpGETRequest(serverName2);
      Serial.println(sensorReadings);        
      Serial.println(serverName2);        
  
  }
}



String httpGETRequest(String serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


int calculateMode(int data[], int dataSize) {
  // Membuat array untuk menghitung frekuensi kemunculan setiap elemen
  int counts[dataSize];

  // Mengisi array counts dengan 0
  for (int i = 0; i < dataSize; i++) {
    counts[i] = 0;
  }

  // Menghitung frekuensi kemunculan setiap elemen
  for (int i = 0; i < dataSize; i++) {
    for (int j = 0; j < dataSize; j++) {
      if (data[j] == data[i]) {
        counts[i]++;
      }
    }
  }

  // Mencari elemen dengan frekuensi kemunculan tertinggi (modus)
  int maxCount = 0;
  int modeValue = -1;

  for (int i = 0; i < dataSize; i++) {
    if (counts[i] > maxCount) {
      maxCount = counts[i];
      modeValue = data[i];
    }
  }

  return modeValue;
}
