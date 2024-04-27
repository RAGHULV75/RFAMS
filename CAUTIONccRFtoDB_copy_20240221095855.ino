#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D8
#define RST_PIN D0
#define LED D2
#define LED_R D4
MFRC522 mfrc522(SS_PIN, RST_PIN);
int statuss = 0;
int out = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
//  your network credentials
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
//  Firebase project API Key
#define API_KEY ""
//  RTDB URLefine the RTDB URL */
#define DATABASE_URL "" 
// Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
String a;
String uid;
void setup(){
  Serial.begin(9600);pinMode(D2,OUTPUT);pinMode(D4,OUTPUT);WiFi.begin(WIFI_SSID, WIFI_PASSWORD);Serial.print("Connecting to Wi-Fi"); digitalWrite(D4, HIGH);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();Serial.print("Connected with IP: ");Serial.println(WiFi.localIP());Serial.println();
  timeClient.begin();timeClient.setTimeOffset(19800);/* time client started */
  SPI.begin();mfrc522.PCD_Init();config.api_key = API_KEY;config.database_url = DATABASE_URL; digitalWrite(D4, LOW);
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("db connected..");
    signupOK = true;
    digitalWrite(D2, HIGH);
    delay(2000);
    digitalWrite(D2, LOW);
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;Firebase.begin(&config, &auth);Firebase.reconnectWiFi(true); Serial.println("RF READY...");
}
void loop(){
// rf loop
  if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
  {
    String uid = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      uid.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println(" : "+ uid );
    if (isAuthorized(uid))
    {
      Serial.println("Authorized card : " + uid);
      digitalWrite(D2, HIGH);
      sendToFirebase(uid);
      digitalWrite(D2, LOW);
      uid = "";
    }
    else
    {
      Serial.println("Un Authorized Card : " + uid);
      digitalWrite(D4, HIGH);
      delay(2000);
      digitalWrite(D4, LOW);
    }
  }// rf loop 
uid ="";
}
bool isAuthorized(String uid)
{
  String authorizedIds [] = { "Verified Card UID 1 "," Verified Card UID 2"};
  for (int i  = 0; i< sizeof(authorizedIds)/sizeof(authorizedIds[0]); i++)
  {
    if (uid == authorizedIds [i])
    {
      return true;
    }
  }
  return false;
}
void sendToFirebase (String uid)
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();// Write an data on the database path test/
    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    String formattedTime = timeClient.getFormattedTime(); //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime); 
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;//Print complete date:
    String currentDate = String(monthDay < 10 ? "0" : "") + String(monthDay) + ":" + String(currentMonth < 10 ? "0" : "") + String(currentMonth) + ":" + String(currentYear);
    Serial.println(formattedTime);
    Serial.println(currentDate);
    int hour = timeClient.getHours();
    if (hour >= 10 && hour < 11) {
      a = "class 1";
    }
    else if (hour >= 11 && hour < 12) {
      a = "class 2";
    }
    else if (hour >= 12 && hour < 13) {
      a = "class 3";
    }
    else if (hour >= 13 && hour < 14) {
      a = "lunch break";
    }
    else if (hour >= 14 && hour < 15) {
      a = "class 4";
    }
    else if (hour >= 15 && hour < 16) {
      a = "class 5";
    } 
    else if (hour >= 16 && hour < 17) {
      a = "class 6";
    }
    else
    {
      a = "before_or_after";
    }
    if (uid =="c3ef8b0d"){
      uid = "REG_Number 1";
    }
    else if (uid =="73e681a6"){
      uid = "REG_Number 2";
    }
  Serial.println(a);
  // rfid set to id // Staff
    if (Firebase.RTDB.setString(&fbdo, "Staff/"+currentDate+"/"+a+"/"+uid, "Present")){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // student
    if (Firebase.RTDB.setString(&fbdo, "Students/"+uid+"/"+currentDate+"/"+a+"/"+uid, "Present")){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
      }}
}
