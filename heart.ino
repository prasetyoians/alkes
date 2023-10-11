#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

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

int menuOption = 0; // Opsi menu saat ini


#define BUTTON_PIN 21    // GIOP21 pin connected to button
#define BUTTON_ATAS 19   // GIOP21 pin connected to button
#define BUTTON_BAWAH 18  // GIOP21 pin connected to button

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


#define REPORTING_PERIOD_MS 200


int sensorDataHeart[REPORTING_PERIOD_MS];  // Array untuk menyimpan data
int sensorDataOxy[REPORTING_PERIOD_MS];    // Array untuk menyimpan data

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

PulseOximeter pox;

uint32_t tsLastReport = 0;




String data = "";
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
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
  pinMode(BUTTON_ATAS, INPUT_PULLUP);
  pinMode(BUTTON_BAWAH, INPUT_PULLUP);
}



void displayMenu() {
  
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      
  
  switch (menuOption) {
    case 0:    
    
      display.println("--> baca hr aja");

      
      display.println("baca spo2 aja");
      
    
      display.println("baca akselo aja");

    
      display.println("baca semua aja");
      break;
    case 1:
      
     
      display.println("baca hr aja");

      
      display.println("--> baca spo2 aja");
      
   
      display.println("baca akselo aja");

     
      display.println("baca semua aja");
      break;
    case 2:
      
     
      display.println("baca hr aja");

     
      display.println("baca spo2 aja");
    
      display.println("--> baca akselo aja");

     
      display.println("baca semua aja");
      break;
    case 3:
      
    
      display.println("baca hr aja");

      
      display.println("baca spo2 aja");
      
     
      display.println("baca akselo aja");

    
      display.println("-->baca semua aja");
      
      break;
  }

  

  display.display();
}


void loop() {

  if (digitalRead(BUTTON_ATAS) == LOW) {
    menuOption--;
    if (menuOption < 0) {
      menuOption = 3;  // Jumlah opsi menu minus satu
    }
    displayMenu();
    delay(200);  // Hindari bouncing tombol
  }

  if (digitalRead(BUTTON_BAWAH) == LOW) {
    menuOption++;
    if (menuOption > 3) {
      menuOption = 0;
    }
    displayMenu();
    delay(200);  // Hindari bouncing tombol
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    // Logika ketika tombol "Select" ditekan
    // Contoh:
    if (menuOption == 0) {
      // Aksi yang terkait dengan opsi pertama
      // etwin ubah kene

    } else if (menuOption == 1) {
      // etwin ubah kene
      
    } else if (menuOption == 2) {
      // nissa ubah kene
   
   
      
    } else if (menuOption == 3) {




      while (!pox.begin()) {
        Serial.println("FAILED");
      }


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

      int aa = 0;
      int bb = 0;
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

      display.print("Akselo: x:");
      display.print(a.acceleration.x, 1);
      display.print(",y:");
      display.print(a.acceleration.y, 1);
      display.print(",z:");
      display.println(a.acceleration.z, 1);
      display.display();
      String serverName2 = "http://127.0.0.1:3000/senddatatosps?hr=" + String(modeValueHeart) + "&spo2=" + String(modeValueOxy) + "&akselox=" + String(a.acceleration.x) + "&akseloy=" + String(a.acceleration.y) + "&akseloz=" + String(a.acceleration.z);

      String sensorReadings = httpGETRequest(serverName2);
      Serial.println(sensorReadings);
      Serial.println(serverName2);
    }
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

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
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

