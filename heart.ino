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

#include <Arduino_JSON.h>

#include <Pangodream_18650_CL.h>
Pangodream_18650_CL BL;

#define ADC_PIN 34
#define CONV_FACTOR 1.7
#define READS 20



#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SIZEMPU 10  //ganti ini jika ingin mengubah size mpu


Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* ssid = "solo";
const char* password = "123412345";


int plus;
Adafruit_MPU6050 mpu;

int menuOption = 4;  // Opsi menu saat ini
int menuBerubah = 1;

#define BUTTON_PIN 27    // GIOP21 pin connected to button
#define BUTTON_ATAS 19   // GIOP21 pin connected to button
#define BUTTON_BAWAH 18  // GIOP21 pin connected to button
const int buzzerPin = 25;  // Change this to your actual buzzer pin



// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin


#define REPORTING_PERIOD_MS 200



int sensorDataAcc[SIZEMPU];  // Array untuk menyimpan data
int hasilAkselo[2];
String hasilAkseloString[1];

int x0 = 0;


int nomor = 0;
unsigned long previousMillis = 0;  // Waktu terakhir kali tugas dilakukan
const long interval = 60000;       //ganti ini jika pengen mengubah waktu mpu

////battery
int sensorValue;
int casIndikator;
float voltage;
int bat_percentage;

unsigned long startTime;
unsigned long currentTime;
const unsigned long measurementInterval = 1000;   // Interval pembacaan setiap detik (1000 ms)
const unsigned long measurementDuration = 10000;  // Durasi pengumpulan data (30 detik)
int analogInPin = 34;                             // Analog input pin
int pinCas = 32;

int measurementCount = 0;
float voltageSum = 0;
float batPercentageSum = 0;
int averageBatPercentage;
int previousBatPercentage = 101;
int previousBatPercentageCas = 0;

int dsp;
///End battery
//// petirCas, 16x16px
const unsigned char petir[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x80, 0x01, 0x80, 0x03, 0xf0,
  0x07, 0xe0, 0x00, 0xc0, 0x00, 0x80, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'wifi by Freepik', 40x40px ICON
const unsigned char wifiIcon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x7f,
  0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff,
  0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xfc, 0x00, 0x3f, 0xfe, 0xff, 0xe0, 0x00, 0x07,
  0xff, 0xff, 0x87, 0xff, 0xe1, 0xff, 0x7f, 0x1f, 0xff, 0xf8, 0xff, 0x7e, 0x7f, 0xff, 0xfe, 0x7e,
  0x3c, 0xff, 0xff, 0xff, 0x3c, 0x01, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xe7, 0xff, 0xc0, 0x03,
  0xfe, 0x00, 0x3f, 0xe0, 0x03, 0xf8, 0x7e, 0x1f, 0xc0, 0x01, 0xe3, 0xff, 0xc7, 0x80, 0x00, 0xc7,
  0xff, 0xe3, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff,
  0xf8, 0x00, 0x00, 0x0f, 0xc3, 0xf0, 0x00, 0x00, 0x07, 0x81, 0xe0, 0x00, 0x00, 0x03, 0x3c, 0xc0,
  0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
  0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
  0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
///heart ICON
static const unsigned char PROGMEM logo2_bmp[] = {
  0x03,
  0xC0,
  0xF0,
  0x06,
  0x71,
  0x8C,
  0x0C,
  0x1B,
  0x06,
  0x18,
  0x0E,
  0x02,
  0x10,
  0x0C,
  0x03,
  0x10,
  0x04,
  0x01,
  0x10,
  0x04,
  0x01,
  0x10,
  0x40,
  0x01,
  0x10,
  0x40,
  0x01,
  0x10,
  0xC0,
  0x03,
  0x08,
  0x88,
  0x02,
  0x08,
  0xB8,
  0x04,
  0xFF,
  0x37,
  0x08,
  0x01,
  0x30,
  0x18,
  0x01,
  0x90,
  0x30,
  0x00,
  0xC0,
  0x60,
  0x00,
  0x60,
  0xC0,
  0x00,
  0x31,
  0x80,
  0x00,
  0x1B,
  0x00,
  0x00,
  0x0E,
  0x00,
  0x00,
  0x04,
  0x00,
};

static const unsigned char PROGMEM logo3_bmp[] = { 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
                                                   0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
                                                   0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
                                                   0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
                                                   0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
                                                   0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
                                                   0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
                                                   0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00 };



/////SP02 ICON 24
const unsigned char oksigen[] PROGMEM = {

  0x30, 0x01, 0xc0, 0x4c, 0x06, 0x30, 0x84, 0x18, 0x0c, 0x80, 0x14, 0x04, 0x44, 0x28, 0x02, 0x6c,
  0x20, 0x02, 0x00, 0x00, 0x01, 0x03, 0xe0, 0x01, 0x0c, 0x18, 0x01, 0x10, 0x04, 0x02, 0x20, 0x02,
  0x02, 0x43, 0x81, 0x04, 0x44, 0xc1, 0x0c, 0x84, 0x40, 0xb0, 0x84, 0x40, 0x80, 0x84, 0x40, 0x80,
  0x84, 0x40, 0x80, 0x44, 0x58, 0x80, 0x46, 0xc9, 0x1c, 0x61, 0x19, 0x22, 0x20, 0x02, 0x21, 0x18,
  0x04, 0x21, 0x0e, 0x18, 0x12, 0x01, 0xe0, 0x0c

};

///32
const unsigned char oksigen1[] PROGMEM = {

  0x0c, 0x00, 0x0f, 0x00, 0x3e, 0x00, 0x3f, 0xc0, 0x41, 0x00, 0xc0, 0x30, 0x41, 0x01, 0x80, 0x18,
  0xc1, 0x83, 0x20, 0x0c, 0x41, 0x02, 0x40, 0x04, 0x41, 0x04, 0x80, 0x02, 0x3e, 0x04, 0x00, 0x02,
  0x08, 0x00, 0x00, 0x02, 0x00, 0x38, 0x00, 0x03, 0x01, 0xff, 0x00, 0x03, 0x06, 0x00, 0xc0, 0x02,
  0x0c, 0x00, 0x60, 0x02, 0x10, 0x00, 0x10, 0x02, 0x30, 0x00, 0x18, 0x04, 0x20, 0xf0, 0x08, 0x0c,
  0x41, 0x98, 0x04, 0x18, 0x41, 0x08, 0x04, 0x30, 0x43, 0x0c, 0x04, 0xc0, 0xc3, 0x0c, 0x06, 0x00,
  0xc3, 0x0c, 0x06, 0x00, 0xc3, 0x0c, 0x06, 0x00, 0x43, 0x0d, 0x84, 0x00, 0x41, 0x09, 0xc4, 0x00,
  0x41, 0x98, 0x84, 0x3c, 0x20, 0xf1, 0x88, 0x46, 0x30, 0x01, 0x98, 0x82, 0x10, 0x00, 0x10, 0x83,
  0x0c, 0x00, 0x60, 0x83, 0x06, 0x00, 0xc0, 0xc2, 0x01, 0xff, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x18
};


// 'jalan 1 res jpg invert', 32x32px
const unsigned char jalan1[] PROGMEM = {
  0x00, 0x01, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x07, 0xf0, 0x00,
  0x00, 0x07, 0xf0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x1f, 0xf0, 0x00,
  0x00, 0x1f, 0xf0, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00,
  0x00, 0x77, 0xfe, 0x00, 0x00, 0x77, 0xfe, 0x00, 0x00, 0x67, 0xf6, 0x00, 0x00, 0x07, 0xf0, 0x00,
  0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00, 0x0f, 0x78, 0x00, 0x00, 0x0e, 0x38, 0x00,
  0x00, 0x1e, 0x38, 0x00, 0x00, 0x1e, 0x3c, 0x00, 0x00, 0x1c, 0x3c, 0x00, 0x00, 0x3c, 0x3c, 0x00,
  0x00, 0x3c, 0x3c, 0x00, 0x00, 0x38, 0x3c, 0x00, 0x00, 0x38, 0x1c, 0x00, 0x00, 0x00, 0x1c, 0x00
};

// 'jalan 2 res jpg invert', 32x32px
const unsigned char jalan2[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xf0, 0x00,
  0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0xc0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x3f, 0xf0, 0x00,
  0x00, 0x3f, 0xf0, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0xe7, 0xff, 0x00, 0x01, 0xef, 0xcf, 0x80,
  0x00, 0xcf, 0xc7, 0x80, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x00,
  0x00, 0x01, 0xe0, 0x00, 0x00, 0x0c, 0xf0, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e, 0x38, 0x00,
  0x00, 0x1c, 0x38, 0x00, 0x00, 0x3c, 0x18, 0x00, 0x00, 0x78, 0x1c, 0x00, 0x00, 0xf0, 0x1c, 0x00,
  0x00, 0xe0, 0x0e, 0x00, 0x00, 0x40, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'pngwing', 32x32px
const unsigned char temp1[] PROGMEM = {
  0x00, 0x01, 0xc0, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x06, 0x30, 0x00, 0x00, 0x05, 0xd0, 0x00,
  0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00,
  0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00,
  0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00,
  0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00, 0x00, 0x05, 0xd0, 0x00,
  0x00, 0x0d, 0xd8, 0x00, 0x00, 0x1d, 0xdc, 0x00, 0x00, 0x1b, 0xcc, 0x00, 0x00, 0x33, 0xe6, 0x00,
  0x00, 0x37, 0xf6, 0x00, 0x00, 0x37, 0xf6, 0x00, 0x00, 0x37, 0xf6, 0x00, 0x00, 0x33, 0xe6, 0x00,
  0x00, 0x19, 0xcc, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x03, 0xe0, 0x00
};
// 'pngwing', 32x32px
const unsigned char temp2[] PROGMEM = {
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00,
  0x00, 0x04, 0x20, 0x00, 0x00, 0x05, 0xa0, 0x00, 0x00, 0x05, 0xa0, 0x00, 0x00, 0x05, 0xa0, 0x00,
  0x00, 0x05, 0xa0, 0x00, 0x00, 0x05, 0xa0, 0x00, 0x00, 0x05, 0xa0, 0x00, 0x00, 0x09, 0x90, 0x00,
  0x00, 0x13, 0xc8, 0x00, 0x00, 0x17, 0xe8, 0x00, 0x00, 0x17, 0xe8, 0x00, 0x00, 0x17, 0xe8, 0x00,
  0x00, 0x17, 0xe8, 0x00, 0x00, 0x19, 0xd8, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x03, 0xc0, 0x00
};

int sensorDataHeart[REPORTING_PERIOD_MS];  // Array untuk menyimpan data
int sensorDataOxy[REPORTING_PERIOD_MS];    // Array untuk menyimpan data
int sensorDataSuhu[10];                    // Array untuk menyimpan data

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);




PulseOximeter pox;

uint32_t tsLastReport = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200);  // Gunakan server NTP yang tersedia

String data = "";
bool customLoopActive = true;

bool customAll = true;

bool cek = true;

void ConnectWIFI() {

  WiFiManager wifiManager;

  // Coba untuk terhubung ke jaringan WiFi yang sudah disimpan
  if (!wifiManager.autoConnect("ALKES")) {
    display.clearDisplay();
    display.drawBitmap(44, 5, wifiIcon, 40, 40, WHITE);

    display.println("Mencari akses poin lain...");
    display.display();

    // Loop untuk mencoba terhubung ke jaringan WiFi baru
    while (!wifiManager.autoConnect()) {
      display.clearDisplay();

      display.setCursor(25, 50);
      display.print("Connecting ...");
      display.display();

      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.drawBitmap(44, 5, wifiIcon, 40, 40, WHITE);
  display.setCursor(33, 50);
  display.print("Connected!");
  display.display();
}
////////////////

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

  // Start WiFiManager for configuration
  // wifiManager.resetSettings();menu
  // WiFiManager wifiManager;

  // wifiManager.autoConnect("ALKES");  // "AutoConnectAP" is the name of the access point
  pinMode(pinCas, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();
  display.drawBitmap(44, 5, wifiIcon, 40, 40, WHITE);

  display.setCursor(0, 45);
  display.println("Sambungkan WiFi anda ke ALKES");
  display.display();
  display.display();

  Serial.println("MPU6050 display demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU6050 sensor");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  ConnectWIFI();
  delay(3000);
  timeClient.begin();
  timeClient.update();

  display.display();

  Serial.print("Initializing pulse oximeter..");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_ATAS, INPUT_PULLUP);
  pinMode(BUTTON_BAWAH, INPUT_PULLUP);

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    // while (1)
    //   ;
  };
  
  pinMode(buzzerPin, OUTPUT);
}
void displayMenu() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // display.print(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
  // display.println("%");
  switch (menuOption) {
    case 0:
      display.fillRect(0, 0, 128, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.println("DETAK JANTUNG");
      display.setTextColor(SSD1306_WHITE);
      display.println("SPO2");
      //display.println("AKSELERASI");
      display.println("SUHU");
      display.println("SEMUA DATA");
      display.println("BERANDA");
      break;
    case 1:
      display.println("DETAK JANTUNG");
      display.fillRect(0, 8, 128, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.println("SPO2");
      display.setTextColor(SSD1306_WHITE);
      //display.println("AKSELERASI");
      display.println("SUHU");
      display.println("SEMUA DATA");
      display.println("BERANDA");
      break;


      break;
    case 2:
      display.println("DETAK JANTUNG");
      display.println("SPO2");
      // display.println("AKSELERASI");

      display.fillRect(0, 16, 128, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.println("SUHU");
      display.setTextColor(SSD1306_WHITE);
      display.println("SEMUA DATA");
      display.println("BERANDA");
      break;
    case 3:
      display.println("DETAK JANTUNG");
      display.println("SPO2");
      // display.println("AKSELERASI");
      display.println("SUHU");

      display.fillRect(0, 24, 128, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.println("SEMUA DATA");
      display.setTextColor(SSD1306_WHITE);
      display.println("BERANDA");
      break;
    case 4:

      display.println("DETAK JANTUNG");
      display.println("SPO2");
      // display.println("AKSELERASI");
      display.println("SUHU");
      display.println("SEMUA DATA");

      display.fillRect(0, 32, 128, 8, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.println("BERANDA");
      break;
  }
  display.display();
}


void loop() {

  Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);


  // Serial.print("Value from pin: ");
  // Serial.println(analogRead(34));
  // Serial.print("Average value from pin: ");
  // Serial.println(BL.pinRead());
  // Serial.print("Volts: ");
  // Serial.println(BL.getBatteryVolts());
  // Serial.print("Charge level: ");
  // Serial.println(BL.getBatteryChargeLevel());


  // Serial.print("Hasil mapping: ");
  // Serial.println(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));
  Serial.println("");
  deteksi_gerak();
  cek_jadwal();


  if (menuBerubah == 1) {
    display_jam_awal();
    if (digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW) {
      menuBerubah = 0;
    }
  }

  if (digitalRead(BUTTON_ATAS) == LOW) {
    menuOption--;
    if (menuOption < 0) {
      menuOption = 4;  // Jumlah opsi menu minus satu
    }
    displayMenu();
    delay(200);  // Hindari bouncing tombol
  }

  if (digitalRead(BUTTON_BAWAH) == LOW) {
    menuOption++;
    if (menuOption > 4) {
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
      display.setTextColor(WHITE);
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
      display.setTextColor(WHITE);
      display.setCursor(10, 3);
      bool logo = true;

      // display.print("DETAK JANTUNG: ");
      if (bb > 0) {
        while (logo) {
          if (digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW) {
            break;
          }
          display.clearDisplay();  // Clear the display

          // Draw and display logo2_bmp
          display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);
          display.println("");
          display.setTextSize(2);
          display.setCursor(0, 40);
          display.print(modeValueHeart);
          display.print(" BPM");
          display.display();
          delay(500);
          display.clearDisplay();  // Clear the display

          // Draw and display logo3_bmp
          display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);
          display.println("");
          display.setTextSize(2);
          display.setCursor(0, 40);
          display.print(modeValueHeart);
          display.print(" BPM");
          display.display();
          delay(500);
        }
      } else {
        display.print("");
      }
      display.println("BPM");
      display.display();

    } else if (menuOption == 1) {
      // Aksi yang terkait dengan opsi pertama
      while (!pox.begin()) {
        Serial.println("FAILED");
      }
      display.clearDisplay();
      Serial.println("Counting...");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Counting...");
      display.display();
      data = "";
      int oxy = 0;
      int bb = 0;

      for (int x = 0; x < REPORTING_PERIOD_MS; x++) {
        pox.update();

        bb = pox.getSpO2();

        if (bb > 0) {
          oxy = oxy + bb;
          Serial.print(".");
          sensorDataOxy[x] = bb;
        }
        delay(REPORTING_PERIOD_MS / 2);
      }

      int dataSizeOxy = sizeof(sensorDataOxy) / sizeof(sensorDataOxy[0]);
      int modeValueOxy = calculateMode(sensorDataOxy, dataSizeOxy);

      // Menampilkan hasil modus di Serial Monitor
      for (int i = 0; i < REPORTING_PERIOD_MS; i++) {
        Serial.print("Data ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(sensorDataHeart[i]);
      }

      Serial.print("Modus dari data heart adalah: ");
      Serial.println(modeValueOxy);

      ///serial
      display.clearDisplay();
      Serial.println("");
      Serial.print("oxy: ");
      if (bb > 0) {
        Serial.print("oxy: ");
        Serial.println(oxy);
        Serial.print("modus: ");


        Serial.print(modeValueOxy);
      } else {
        Serial.print("");
      }
      Serial.println("%");

      ///display
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 3);
      bool logo = true;

      // display.print("DETAK JANTUNG: ");
      if (bb > 0) {
        while (logo) {
          if (digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW) {
            break;
          }
          display.clearDisplay();  // Clear the display

          // Draw and display logo2_bmp
          display.drawBitmap(5, 5, oksigen, 24, 24, WHITE);
          display.println("");
          display.setTextSize(2);
          display.setCursor(0, 40);
          display.print(modeValueOxy);
          display.print(" %");
          display.display();
          delay(500);
          display.clearDisplay();  // Clear the display

          // Draw and display logo3_bmp
          display.drawBitmap(0, 0, oksigen1, 32, 32, WHITE);
          display.println("");
          display.setTextSize(2);
          display.setCursor(0, 40);
          display.print(modeValueOxy);
          display.print(" %");
          display.display();
          delay(500);
        }
      } else {
        display.print("");
      }
      display.println("%");
      display.display();

    } else if (menuOption == 2) {

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);

      bool logo = true;
      while (logo) {
        if (digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW) {
          break;
        }
        int cc = 0;
        for (int x = 0; x < 10; x++) {


          cc = mlx.readObjectTempC();

          Serial.print(".");
          sensorDataSuhu[x] = cc;

          delay(50);
        }
        int dataSizeSuhu = sizeof(sensorDataSuhu) / sizeof(sensorDataSuhu[0]);

        int modeValueSuhu = calculateMode(sensorDataSuhu, dataSizeSuhu);
        display.clearDisplay();  // Clear the display

        // Draw and display logo2_bmp
        display.drawBitmap(0, 0, temp2, 32, 32, WHITE);
        display.println("");
        display.setTextSize(2);
        display.setCursor(0, 40);

        display.print(modeValueSuhu);
        display.println(" C");

        display.display();
        delay(500);
        display.clearDisplay();  // Clear the display

        // Draw and display logo3_bmp
        display.drawBitmap(0, 0, temp1, 32, 32, WHITE);
        display.println("");
        display.setTextSize(2);
        display.setCursor(0, 40);

        display.print(modeValueSuhu);
        display.println(" C");

        display.display();
        delay(500);
      }

    } else if (menuOption == 3) {
      while (!pox.begin()) {
        Serial.println("FAILED");
      }

      display.clearDisplay();
      Serial.println("Counting...");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Counting...");
      display.display();
      data = "";

      int oxy = 0, countOxy = 0;
      int heart = 0;
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
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("DETAK JANTUNG: ");
      if (bb > 0) {
        display.print(modeValueHeart);
      } else {
        display.print("");
      }
      display.println("BPM ");
      display.println("");

      display.print("TINGKAT OKSIGEN: ");
      if (aa > 0) {
        display.print(modeValueOxy);
      } else {
        display.print("");
      }
      display.println("%");
      display.println("");
      //mlx
      display.print("SUHU:");
      display.print(mlx.readObjectTempC());
      display.println("C");
      int suhus = mlx.readObjectTempC();
      display.println("");
      // mpu
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      // display.println("AKSELOMETER :");
      // display.print("-X: ");
      // display.println(a.acceleration.x, 1);
      // display.print("-Y:");
      // display.println(a.acceleration.y, 1);
      // display.print("-Z:");
      // display.println(a.acceleration.z, 1);
      display.display();

      WiFiClient client;
      HTTPClient http;      
      String serverName2 = "https://node-heart-prasetyoians.vercel.app/senddatatosps";
      String enc = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtYSI6ImlhbiIsImlhdCI6MTUxNjIzOTAyMn0.433M2X-k55i05iqwf0RPZXAy7oc3vKfIEcruSqMOUs8";
      // url += "?hr=70&spo2=96&akselox=1&akseloy=2&akseloz=1&suhu=36";
      serverName2 += "?hr=" + String(modeValueHeart) + "&spo2=" + String(modeValueOxy) + "&akselox=" + String(a.acceleration.x) + "&akseloy=" + String(a.acceleration.y) + "&akseloz=" + String(a.acceleration.z) + "&suhu=" + String(suhus)+ "&encoded=" + String(enc);
      // String serverName2 = "https://node-heart-prasetyoians.vercel.app/senddatatosps?hr=" + String(modeValueHeart) + "&spo2=" + String(modeValueOxy) + "&akselox=" + String(a.acceleration.x) + "&akseloy=" + String(a.acceleration.y) + "&akseloz=" + String(a.acceleration.z) + "&suhu=" + String(suhus);

      Serial.print("Sending GET request to: ");
      Serial.println(serverName2);


      // Your Domain name with URL path or IP address with path
      http.begin(serverName2);


      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Send HTTP POST request
      int httpResponseCode = http.GET();

      // String payload = "{}";
      if (httpResponseCode == 308) {
        Serial.println("Received a redirect response. Following new location.");

        // Dapatkan lokasi baru dari header "Location"
        String newLocation = http.header("Location");
        Serial.print("New location: ");
        Serial.println(newLocation);

        // Hentikan koneksi saat ini
        http.end();

        // Buat koneksi baru ke lokasi baru
        http.begin(newLocation);
        httpResponseCode = http.GET();
      }
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Baca respons dari server
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.print("HTTP Request failed. Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();

      Serial.println(serverName2);


    } else if (menuOption == 4) {
      while (customAll) {
        display_jam_awal();
        // Penambahan deteksi_gerak dan delay diganti 200
        deteksi_gerak();
        if (digitalRead(BUTTON_ATAS) == LOW || digitalRead(BUTTON_BAWAH) == LOW) {
          break;

          //TRUE
          //CONDITION  OR FALSE
        }
      }
      delay(200);
    }
    Serial.println(menuOption);
  }
  x0++;
}

void cek_jadwal() {

  HTTPClient http;

  const char* serverName = "http://192.168.43.76:3000/cek_jadwal";
  String sensorReadings;
  //float sensorReadingsArr[3];

  sensorReadings = httpGETRequest(serverName);
  Serial.println(sensorReadings);
  JSONVar myObject = JSON.parse(sensorReadings);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("JSON object = ");
  Serial.println(myObject);

  // myObject.keys() can be used to get an array of all the keys in the object

  Serial.print("stat:");
  Serial.println(myObject[0]["json_response"]);
  Serial.println(myObject);

  
  String stat = myObject[0]["json_response"];

  if (stat== "1") {


     digitalWrite(buzzerPin, HIGH);
      delay(800);
      digitalWrite(buzzerPin, LOW);
      delay(800);
      
       digitalWrite(buzzerPin, HIGH);
      delay(800);
      digitalWrite(buzzerPin, LOW);
      delay(800);
      

    display.clearDisplay();

    display.setCursor(25, 50);
    display.print("WAKTUNYA UNTUK :");
      display.display();
  
  // Now you can parse the JSON response
  // Use a JSON parsing library or manual parsing based on the structure of your JSON
}
http.end();


}


void deteksi_gerak() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int totalAcc = sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z);
  Serial.print("Total Akselerasi ke-");
  Serial.print(x0);
  Serial.print(": ");
  Serial.println(totalAcc);

  sensorDataAcc[x0] = totalAcc;


  unsigned long currentMillis = millis();  // Mendapatkan waktu saat ini

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int dataSizeAcc = sizeof(sensorDataAcc) / sizeof(sensorDataAcc[0]);
    int modeValueAcc = calculateMode(sensorDataAcc, dataSizeAcc);



    Serial.print("Modus dari data Acc adalah: ");
    Serial.println(modeValueAcc);


    int nomor;
    String keterangan;
    if (modeValueAcc >= 20) {
      Serial.println("Anda Banyak bergerak dengan gerakan yang aktif");
      nomor = 3;
      keterangan = "Aktif";
    } else if (modeValueAcc >= 11 && modeValueAcc < 20) {
      Serial.println("Anda Lumayan banyak bergerak bergerak tapi harus ditingkatkan");
      nomor = 2;
      keterangan = "Normal";

    } else if (modeValueAcc <= 10) {
      Serial.println("Anda Kurang Gerak!");
      nomor = 1;
      keterangan = "Tidak";
    }
    /////////////

    WiFiClient client;
    HTTPClient http;
    String serverName3 = "https://node-heart-prasetyoians.vercel.app/sendAkselo";
    // url += "?hr=70&spo2=96&akselox=1&akseloy=2&akseloz=1&suhu=36";
    serverName3 += "?modus=" + String(modeValueAcc) + "&nomor=" + String(nomor);
    // String serverName2 = "https://node-heart-prasetyoians.vercel.app/senddatatosps?hr=" + String(modeValueHeart) + "&spo2=" + String(modeValueOxy) + "&akselox=" + String(a.acceleration.x) + "&akseloy=" + String(a.acceleration.y) + "&akseloz=" + String(a.acceleration.z) + "&suhu=" + String(suhus);

    Serial.print("Sending GET request to: ");
    Serial.println(serverName3);


    // Your Domain name with URL path or IP address with path
    http.begin(serverName3);


    // If you need Node-RED/server authentication, insert user and password below
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    // String payload = "{}";
    if (httpResponseCode == 308) {
      Serial.println("Received a redirect response. Following new location.");

      // Dapatkan lokasi baru dari header "Location"
      String newLocation = http.header("Location");
      Serial.print("New location: ");
      Serial.println(newLocation);

      // Hentikan koneksi saat ini
      http.end();

      // Buat koneksi baru ke lokasi baru
      http.begin(newLocation);
      httpResponseCode = http.GET();
    }
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Baca respons dari server
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("HTTP Request failed. Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    Serial.println(serverName3);

    ////////////
    // WiFiClient client3;
    // HTTPClient http3;
    // String serverName3 = "https://node-heart-prasetyoians.vercel.app/sendAkselo?modus=" + String(modeValueAcc) + "&nomor=" + String(nomor);
    // // Your Domain name with URL path or IP address with path
    // http3.begin(client3, serverName3);

    // // If you need Node-RED/server authentication, insert user and password below
    // //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

    // // Send HTTP POST request
    // int httpResponseCode3 = http3.GET();
    // Serial.println(httpResponseCode3);

    // String payload3 = "{}";


    // // Free resources
    // http3.end();

    // Serial.println(serverName3);
  }



  if (x0 == SIZEMPU) {





    sensorDataAcc[SIZEMPU] = {};
    x0 = 0;
  }
  // x0++;
  //delay(1000);  // Sesuaikan delay sesuai kebutuhan
}



void resetArray(int arr[], int size) {
  for (int i = 0; i < size; i++) {
    arr[i] = 0;
  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void updateDisplay(String timeStr, String dateStr) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  int screenWidth = 128;  // Lebar layar display
  // Mengatur koordinat x berdasarkan lebar layar
  int xRect = screenWidth - 3.5 - 20;
  xRect -= 5;
  int xText = screenWidth - 3.5 - 20;
  int xText2 = screenWidth - 0.5 - 24;

  display.drawRect(xRect, 5, 25, 10, WHITE);
  display.drawRect(xRect + 25, 7.5, 2.5, 5, WHITE);

  display.setTextColor(WHITE);
  display.setTextSize(1);
  sensorValue = analogRead(analogInPin);
  casIndikator = digitalRead(pinCas);
  voltage = mapfloat(sensorValue, 2090, 1670, 3.7, 3.0);
  bat_percentage = mapfloat(sensorValue, 2090, 1670, 100, 0);

  // Serial.print("Cas Value = ");
  // Serial.print(casIndikator);
  // Serial.print("\t Analog Value = ");
  // Serial.print(sensorValue);
  // // Serial.print("\t Output Voltage = ");
  // // Serial.print(voltage);
  // Serial.print("\t Battery Percentage = ");
  // Serial.println(bat_percentage);
  if (dsp == 1) {
    display.setCursor(xText2, 5 + 2);
    display.print(averageBatPercentage);
    display.drawBitmap(xText2 - 25, 1 + 1, petir, 16, 16, WHITE);
  } else if (dsp == 2) {
    display.setCursor(xText2, 5 + 2);
    display.print(previousBatPercentageCas);
    display.drawBitmap(xText2 - 25, 1 + 1, petir, 16, 16, WHITE);
  } else if (dsp == 3) {
    display.setCursor(xText2, 5 + 2);
    display.print(averageBatPercentage);
  } else if (dsp == 4) {
    display.setCursor(xText2, 5 + 2);
    display.print(previousBatPercentage);
  } else {
    Serial.print("Tidak Kebaca!!!!");
  }
  currentTime = millis();
  if (casIndikator > 0) {
    if (currentTime - startTime < measurementDuration) {
      if (currentTime - startTime >= measurementCount * measurementInterval) {
        voltageSum += voltage;
        batPercentageSum += bat_percentage;
        measurementCount++;
      }
    } else {
      if (measurementCount > 0) {
        float averageVoltage = voltageSum / measurementCount;
        averageBatPercentage = batPercentageSum / measurementCount;

        // Serial.print("Average Voltage = ");
        // Serial.print(averageVoltage);

        if (averageBatPercentage > previousBatPercentageCas) {
          dsp = 1;
          // Update previousBatPercentage jika averageBatPercentage lebih kecil
          if (averageBatPercentage < 0) {
            averageBatPercentage = -averageBatPercentage;
            if (averageBatPercentage > 100) {
              averageBatPercentage = 100;
            }
          }

          previousBatPercentageCas = averageBatPercentage;
          previousBatPercentage = previousBatPercentageCas;
          Serial.print("\t Average Battery Percentage (Updated CAS) = ");
          Serial.println(averageBatPercentage);

        } else {
          dsp = 2;
          Serial.print("\t Average Battery Percentage (CAS) = ");
          Serial.println(previousBatPercentageCas);
        }
      }
      // Reset variabel untuk pengukuran berikutnya
      startTime = millis();
      if (averageBatPercentage > previousBatPercentageCas) {

      } else {
      }
      measurementCount = 0;
      voltageSum = 0;
      batPercentageSum = 0;
    }
  } else {
    if (currentTime - startTime < measurementDuration) {
      if (currentTime - startTime >= measurementCount * measurementInterval) {
        voltageSum += voltage;
        batPercentageSum += bat_percentage;
        measurementCount++;
      }
    } else {
      if (measurementCount > 0) {
        float averageVoltage = voltageSum / measurementCount;
        averageBatPercentage = batPercentageSum / measurementCount;

        // Serial.print("Average Voltage = ");
        // Serial.print(averageVoltage);

        if (averageBatPercentage < previousBatPercentage) {
          dsp = 3;
          // Update previousBatPercentage jika averageBatPercentage lebih kecil
          if (averageBatPercentage < 0) {
            averageBatPercentage = -averageBatPercentage;
            if (averageBatPercentage > 100) {
              averageBatPercentage = 100;
            }
          }
          previousBatPercentage = averageBatPercentage;
          previousBatPercentageCas = previousBatPercentage;
          Serial.print("\t Average Battery Percentage (Update) = ");
          Serial.println(averageBatPercentage);

        } else {
          dsp = 4;
          Serial.print("\t Average Battery Percentage (NORMAL) = ");
          Serial.println(previousBatPercentage);
        }
      }

      // Reset variabel untuk pengukuran berikutnya
      startTime = millis();
      if (averageBatPercentage < previousBatPercentage) {
        // Serial.println(averageBatPercentage);
        // display.setCursor(xText2, 5 + 2);
        // display.print(averageBatPercentage);
      } else {
        // Serial.println(previousBatPercentage);
        // display.setCursor(xText2, 5 + 2);
        // display.print(previousBatPercentage);
      }
      measurementCount = 0;
      voltageSum = 0;
      batPercentageSum = 0;
    }
  }

  // Menempatkan teks "battery" di sebelah kanan objek
  // display.setCursor(xText2, 5 + 2);
  // display.print(map(BL.getBatteryChargeLevel(), 0, 10, 0, 100));

  display.println("");

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println(dateStr);

  // display_battery();
  display.setTextSize(3);
  display.setCursor(0, 25);

  display.println(timeStr);
  display.display();
  // delay(1000);
}

void display_jam_awal() {
  unsigned long lastMinute = 61;  // Inisialisasi dengan angka yang tidak mungkin dalam menit (1 lebih dari 60)
  time_t epochTime = timeClient.getEpochTime();

  struct tm* ptm = gmtime((time_t*)&epochTime);

  timeClient.update();  // Memperbarui waktu dari server NTP
  int day = ptm->tm_mday;
  int month = ptm->tm_mon + 1;
  int year = ptm->tm_year + 1900;

  // Periksa apakah menit telah berubah
  if (timeClient.getSeconds() > 0 && timeClient.getMinutes() != lastMinute) {
    lastMinute = timeClient.getMinutes();

    String mins;
    if (timeClient.getMinutes() < 10) {
      mins = "0" + String(timeClient.getMinutes());
    } else {
      mins = String(timeClient.getMinutes());
    }

    String timeStr = String(timeClient.getHours()) + ":" + mins;
    String dateStr = String(day) + "/" + String(month) + "/" + String(year);

    updateDisplay(timeStr, dateStr);

    Serial.println(timeStr);
  }

  delay(1000);  // Tunggu selama 1 detik sebelum memeriksa kembali
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
