#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 

#define SS_PIN D4
#define RST_PIN D2

#ifndef STASSID
#define STASSID "YOUR-WIFI-NAME"
#define STAPSK  "YOUR-WIFI-PASSWORD"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Reading RFID");
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UID: ");
      // Answer from Hans Kilian in https://stackoverflow.com/questions/32839396/how-to-get-the-uid-of-rfid-in-arduino
      unsigned long hex_num;
      hex_num =  mfrc522.uid.uidByte[0] << 24;
      hex_num += mfrc522.uid.uidByte[1] << 16;
      hex_num += mfrc522.uid.uidByte[2] <<  8;
      hex_num += mfrc522.uid.uidByte[3];
      Serial.println(hex_num);
      Serial.println();
      mfrc522.PICC_HaltA();
    } 
  }
}
