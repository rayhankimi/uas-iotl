#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Firebase settings
#define DATABASE_URL "https://cepsto-3b012-default-rtdb.firebaseio.com/" // Ganti dengan host Firebase Anda
#define API_KEY "MwhxTXuWd1PNQCAowmMcisFtvr9IDHAssHq0q2kg" // Ganti dengan secret Firebase Anda

// WiFi settings
#define WIFI_SSID "Rumah." // Ganti dengan SSID WiFi Anda
#define WIFI_PASS "KunciT12" // Ganti dengan password WiFi Anda

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;

String databasePath;
String tdsPath;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;


// Tentukan pin analog yang terhubung dengan sensor TDS
const int TdsSensorPin = 27;

float voltage, tdsValue, temperature = 25;

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Koneksi Wifi wi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void sendFloat(String path, float value){
    if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
        Serial.println("TDS Value is : ");
        Serial.println(value);
        Serial.print(" On Path : ");
        Serial.println(path);
        Serial.println("LOLOS WI SUKSMA");
        Serial.println("PATH " + fbdo.dataPath());
        Serial.println("TYPE " + fbdo.dataType());
    } else {
        Serial.println("Set float data gagal wi");
        Serial.println("Error: " + fbdo.errorReason());
    }
}

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);
  initWifi();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Connected Wi");
    }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  databasePath = "/UsersData/" + uid;
  tdsPath = databasePath + "/TDS";

  
}

void loop() {
  int analogValue = analogRead(TdsSensorPin);
  voltage = analogValue * (3.3 / 4095.0); // ESP32 menggunakan ADC 12-bit (0-4095)
  tdsValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * 0.5;
  
  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print("  Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V  TDS Value: ");
  Serial.print(tdsValue, 0);
  Serial.println(" ppm");
  
  // Kirim data ke Firebase
  if (Firebase.ready() &&  (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Get latest sensor readings
    sendFloat(tdsPath, tdsValue);
}}