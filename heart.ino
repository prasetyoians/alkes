#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#include "Adafruit_GFX.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <NTPClient.h>

#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>


#include <Adafruit_MLX90614.h>

#include <Pangodream_18650_CL.h>
Pangodream_18650_CL BL;

#define ADC_PIN 34
#define CONV_FACTOR 1.7
#define READS 20


Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* ssid = "solo";
const char* password = "123412345";


int plus;
Adafruit_MPU6050 mpu;

int menuOption = 5;  // Opsi menu saat ini


#define BUTTON_PIN 21    // GIOP21 pin connected to button
#define BUTTON_ATAS 19   // GIOP21 pin connected to button
#define BUTTON_BAWAH 18  // GIOP21 pin connected to button

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


#define REPORTING_PERIOD_MS 200


int sensorDataHeart[REPORTING_PERIOD_MS];  // Array untuk menyimpan data
int sensorDataOxy[REPORTING_PERIOD_MS];    // Array untuk menyimpan data
int sensorDataSuhu[10];                    // Array untuk menyimpan data

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

PulseOximeter pox;

uint32_t tsLastReport = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);  // Gunakan server NTP yang tersedia



String data = "";
bool customLoopActive = true;

      bool customAll = true;



void display_battery() {
  display.clearDisplay();

  int screenWidth = 128;  // Lebar layar display
  // Mengatur koordinat x berdasarkan lebar layar
  int xRect = screenWidth - 3.5 - 20;
  xRect -= 5;
  int xText = screenWidth - 3.5 - 20;
  int xText2 = screenWidth - 0.5 - 15;

  display.drawRect(xRect, 5, 25, 10, WHITE);
  display.drawRect(xRect + 25, 7.5, 2.5, 5, WHITE);
  display.fillRect(xRect, 5, (2 * map(BL.getBatteryChargeLevel(), 0, 10, 0, 100) / 8), 10, WHITE);
  if (map(BL.getBatteryChargeLevel(), 0, 10, 0, 100) <= 50) {
    // Mengatur warna teks menjadi putih
    display.setTextColor(WHITE);
    display.setTextSize(1);

    // Menempatkan teks "battery" di sebelah kanan objek
    display.setCursor(xText2, 5 + 2);
    display.print(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
    display.display();
    delay(2000);
  } else {
    // Mengatur warna teks menjadi hitam
    display.setTextColor(BLACK);
    display.setTextSize(1);

    // Menempatkan teks "battery" di sebelah kanan objek
    display.setCursor(xText, 5 + 2);
    display.print(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
    display.display();
    delay(2000);
  }

  String formattedTime = timeClient.getFormattedTime();  // Assuming formattedTime is in format "HH:MM:SS"

  int colonPos = formattedTime.indexOf(':');

  // Extract hour substring
  String hourString = formattedTime.substring(0, colonPos);

  // Extract minute substring
  String minuteString = formattedTime.substring(colonPos + 1, colonPos + 3);  // Assumes minutes are always two digits

  int hour = hourString.toInt();      // Convert string to integer
  int minute = minuteString.toInt();  // Convert string to integerg
}

void setup() {
  Serial.begin(115200);
  // WiFi.begin(ssid, password);
  // Serial.println("Connecting");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.print("Connected to WiFi network with IP Address: ");
  // Serial.println(WiFi.localIP());
  WiFiManager wifiManager;

  // Start WiFiManager for configuration
  // wifiManager.resetSettings();

  wifiManager.autoConnect("ALKES");  // "AutoConnectAP" is the name of the access point

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  Serial.println("MPU6050 display demo");

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

  timeClient.begin();
  timeClient.update();

  display_jam_awal();
  display.display();




  Serial.print("Initializing pulse oximeter..");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ATAS, INPUT_PULLUP);
  pinMode(BUTTON_BAWAH, INPUT_PULLUP);

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };
}



void displayMenu() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // display.print(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
  // display.println("%");

int SCREEN_WIDTH = 128;
int SCREEN_HEIGHT = 32;

  switch (menuOption) {
    case 0:

      display.fillRect(0, 0, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 
      display.println("DETAK JANTUNG");


display.setTextColor(SSD1306_WHITE); 
      display.println("SPO2");


      display.println("AKSELERASI");

      display.println("SUHU");


      // display.println("Semua Data");
      // display.println("Beranda");
      break;
    case 1:


      display.println("DETAK JANTUNG");


      display.fillRect(0, 8, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 

      display.println("SPO2");
      display.setTextColor(SSD1306_WHITE); 

      


      display.println("AKSELERASI");

      display.println("SUHU");




      // display.println("Semua Data");
      // display.println("Beranda");

      break;
    case 2:


      display.println("DETAK JANTUNG");


      display.println("SPO2");


      display.fillRect(0, 16, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 

      display.println("AKSELERASI");


      display.setTextColor(SSD1306_WHITE); 

      display.println("SUHU");


      // display.println("Semua Data");
      // display.println("Beranda");

      break;
    case 3:


      display.println("DETAK JANTUNG");


      display.println("SPO2");


      display.println("AKSELERASI");



      display.fillRect(0, 24, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 
      
      display.println("SUHU");


      // display.println("-->Semua Data");
      // display.println("Beranda");


      break;
    case 4:


      display.println("SPO2");


      display.println("AKSELERASI");
      display.println("SUHU");



      display.fillRect(0, 24, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 
      display.println("SEMUA DATA");
      // display.println("Beranda");


      break;


    case 5:


      // display.println("SPO2");


      display.println("AKSELERASI");
      display.println("SUHU");



      display.println("SEMUA DATA");


      
      display.fillRect(0, 24, 128, 8, SSD1306_WHITE); 
      display.setTextColor(SSD1306_BLACK); 
      display.println("BERANDA");


      break;
  }



  display.display();
}


void loop() {

  Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);


  Serial.print("Value from pin: ");
  Serial.println(analogRead(34));
  Serial.print("Average value from pin: ");
  Serial.println(BL.pinRead());
  Serial.print("Volts: ");
  Serial.println(BL.getBatteryVolts());
  Serial.print("Charge level: ");
  Serial.println(BL.getBatteryChargeLevel());
  // Serial.print("Hasil mapping: ");
  // Serial.println(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
  Serial.println("");


  if (digitalRead(BUTTON_ATAS) == LOW) {
    menuOption--;
    if (menuOption < 0) {
      menuOption = 5;  // Jumlah opsi menu minus satu
    }
    displayMenu();
    delay(200);  // Hindari bouncing tombol
  }

  if (digitalRead(BUTTON_BAWAH) == LOW) {
    menuOption++;
    if (menuOption > 5) {
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
      int heart = 0;
      int bb = 0;

      for (int x = 0; x < REPORTING_PERIOD_MS; x++) {
        pox.update();

        bb = pox.getHeartRate();

        if (bb > 0) {
          heart = heart + bb;
          Serial.print(".");
          sensorDataHeart[x] = bb;
        }
        delay(REPORTING_PERIOD_MS / 2);
      }

      int dataSizeHeart = sizeof(sensorDataHeart) / sizeof(sensorDataHeart[0]);
      int modeValueHeart = calculateMode(sensorDataHeart, dataSizeHeart);

      // Menampilkan hasil modus di Serial Monitor
      for (int i = 0; i < REPORTING_PERIOD_MS; i++) {
        Serial.print("Data ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(sensorDataHeart[i]);
      }

      Serial.print("Modus dari data heart adalah: ");
      Serial.println(modeValueHeart);

      ///serial
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

      ///display
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(1);
      display.setCursor(0, 0);


      display.print("hr: ");
      if (bb > 0) {
        display.print(modeValueHeart);
      } else {
        display.print("");
      }
      display.println("BPM");
      display.display();

    } else if (menuOption == 1) {
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

      // int posX = 0, posY = 0, posZ = 0;
      //

      int aa = 0;
      // baca pox
      for (int x = 0; x < REPORTING_PERIOD_MS; x++) {
        pox.update();

        aa = pox.getSpO2();

        if (aa > 0) {
          countOxy++;
          oxy = oxy + aa;
          Serial.print(".");
          sensorDataOxy[x] = aa;
        }
        delay(REPORTING_PERIOD_MS / 2);
      }


      int dataSizeOxy = sizeof(sensorDataOxy) / sizeof(sensorDataOxy[0]);


      int modeValueOxy = calculateMode(sensorDataOxy, dataSizeOxy);

      // Menampilkan hasil modus di Serial Monitor

      display.clearDisplay();
      Serial.println("");

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
      display.print("oxy: ");
      if (aa > 0) {
        display.print(modeValueOxy);
      } else {
        display.print("");
      }
      display.println("%");
      display.display();

    } else if (menuOption == 2) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(1);
      display.setCursor(0, 0);

      display.print("Akselo: x:");
      display.println(a.acceleration.x, 1);
      display.print(",y:");
      display.println(a.acceleration.y, 1);
      display.print(",z:");
      display.println(a.acceleration.z, 1);
      delay(100);
      display.display();

      //


    } else if (menuOption == 3) {

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(1);
      display.setCursor(0, 0);



      display.println("suhu:");



      int cc = 0;
      for (int x = 0; x < 10; x++) {


        cc = mlx.readObjectTempC();

        Serial.print(".");
        sensorDataSuhu[x] = cc;

        delay(50);
      }

      int dataSizeSuhu = sizeof(sensorDataSuhu) / sizeof(sensorDataSuhu[0]);

      int modeValueSuhu = calculateMode(sensorDataSuhu, dataSizeSuhu);

      display.println(modeValueSuhu);

      display.display();







    } else if (menuOption == 4) {




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

        if (bb > 0) {
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
      display.print("hr: ");
      if (bb > 0) {
        display.print(modeValueHeart);
      } else {
        display.print("");
      }
      display.print("BPM, ");


      display.print("oxy: ");
      if (aa > 0) {
        display.print(modeValueOxy);
      } else {
        display.print("");
      }
      display.println("%");

      //mlx
      display.print("suhu:");
      display.print(mlx.readObjectTempC());
      display.println("C");


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



    } else if (menuOption == 5) {


          customAll != customAll;

          

        while(customAll){
          display_jam_awal();
        delay(500);

          if(digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW ){
            
              break;                          
          }
        }

       
    }

    Serial.println(menuOption);
  }
}


void display_jam_awal() {


    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(1);
    display.setCursor(0, 0);

    timeClient.begin();
    timeClient.update();


    display_battery();

    display.setTextSize(2);

    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();


    display.print(currentHour);
    display.print(":");
    display.println(currentMinute);


    delay(1000);
    display.display();
  
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
