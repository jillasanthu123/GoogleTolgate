
#include <Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include "time.h"

const char* ntpServer="pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;


#define SS_PIN 5//for esp32
#define RST_PIN 21

MFRC522 rfid(SS_PIN, RST_PIN);

int car=1;
//Wifi
const char* ssid="Redmi 9 Power";
const char* password="12345678";
//google script
String GOOGLE_SCRIPT_ID ="AKfycbxtTmPYU2GHEkgS2ynjPrIxIIpwFqI8hBhxlsGgbvgA8el6EjmJ_E-Z9b5WH4nYiASx0A";

int count=1;
String name="Sai";
int sensor_pin=4;
void setup() {
  // put your setup code here, to run once:
       Serial.begin(9600);
        pinMode(sensor_pin, INPUT);
  delay(1000);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
     SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522

//  Serial.println("Tap RFID/NFC Tag on reader");  
Serial.println("Wifi connectted"); 
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {
  
  // put your main code here, to run repeatedly:
      String str="";
      while(digitalRead(sensor_pin)==0 && WiFi.status() == WL_CONNECTED){
      
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    Serial.println("card is present");
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print NUID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {

        Serial.print(rfid.uid.uidByte[i]);
        str=str+String(rfid.uid.uidByte[i]);

      }
      Serial.println("String is"+str);

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
else{
  Serial.println("place card");
  delay(500);
}

  if(str=="351902426"){
    name="Jilla";
  }
  else if(str=="1942278927"){
    name="K.Santosh";
  }
  else if(str=="10116423333"){
    name="Bear Grills";
  }
  else if(str=="16182228"){
     name="Twice";
  }
  else{
    name=str;
  }
String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+ "id=" + String(str);

Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
   int httpCode =  http.GET(); 

    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
  //  ---------------------------------------------------------------------
    http.end();

}



}
