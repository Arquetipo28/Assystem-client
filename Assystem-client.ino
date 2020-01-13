
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SS_PIN D4
#define RST_PIN D2

#ifndef STASSID
#define STASSID "Totalplay-A99E_EXT"
#define STAPSK  "A99EA7E23cqYYGQC"
#endif

const char* ssid      = STASSID;
const char* password  = STAPSK;
const char* host      = "http://api-vp2mx-com.herokuapp.com/api/v1/entries/create";

const char* location  = "S01";
const char* section   = "SECTION_ID";
const int   green_led = D1;
const int   red_led   = D3;

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
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  digitalWrite(red_led, HIGH);
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
      String a = postValues(hex_num);
      Serial.println(a);
      Serial.println();
      mfrc522.PICC_HaltA();
      digitalWrite(red_led, LOW);
      digitalWrite(green_led, HIGH);
      delay(500);
      digitalWrite(green_led, LOW);
      digitalWrite(red_led, HIGH);
    }
  }
}

String postValues(unsigned long card) {

  HTTPClient http;   //Declare object of class HTTPClient
  http.begin(host); //Specify request destination
  http.addHeader("Content-Type", "application/json"); //Specify content-type header
  http.addHeader("Accept", "application/json");
  Serial.println(card);
  String json = "{\"entry\":{ \"card_id\":\"" + String(card) + "\", \"lector\":\"" + location + "\"}, \"token\":\"OP-ASSYSTEM-AP2\" }";
  int httpCode = http.POST(json);  //Send the request
  String payload = http.getString(); //Get the response payload
  if (httpCode == 200) {
    Serial.print("Response: ");
    Serial.print(httpCode);
    Serial.println();
    return payload;
  } else {
    return "Error";
  }
}
