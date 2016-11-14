#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <string.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define ON      HIGH
#define OFF     LOW
#define STATE_A HIGH
#define STATE_B LOW

int LoadStatus = ON;
char nodeCMD[]= "wb.toggle.l.1";

int switchState = ON;

// Port Define
int relayA = 4;
int relayB = 5;
int AC_Sense = 12;

int AC_Sense_readState = ON;

void toggleLoad(int *LoadStatus, int *switchState) {
  if(*switchState == STATE_A) {
      if(*LoadStatus == ON) {
          *LoadStatus = OFF;
          Serial.printf("CASE: 1\n");
          Serial.printf("Load OFF\n");
          digitalWrite(relayA, HIGH);
          digitalWrite(relayB, LOW);
          delay(200);
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, LOW);
    } else {
          *LoadStatus = ON;
          Serial.printf("CASE: 2\n");
          Serial.printf("Load ON\n");
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, HIGH);
          delay(200);
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, LOW);
    }
  }else {
      if(*LoadStatus == ON) {
          *LoadStatus = OFF;
          Serial.printf("CASE: 3\n");
          Serial.printf("Load OFF\n");
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, HIGH);
          delay(200);
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, LOW);
      } else {
          *LoadStatus = ON;
          Serial.printf("CASE: 4\n");
          Serial.printf("Load ON\n");
          digitalWrite(relayA, HIGH);
          digitalWrite(relayB, LOW);
          delay(200);
          digitalWrite(relayA, LOW);
          digitalWrite(relayB, LOW);
      }
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("Disconnected from web Socket\n");
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to Web Socket\n");
            break;
        
        case WStype_TEXT:
            if(strcmp((const char*)payload, nodeCMD) == 0){
                Serial.printf("\n[WS] Receive: %s\n", payload);
                toggleLoad(&LoadStatus, &switchState);
              }
            break;
            
        case WStype_BIN:
            Serial.printf("[WS] get binary lenght: %u\n", lenght);
            //hexdump(payload, lenght);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    //IO Setup
    pinMode(relayA, OUTPUT);
    pinMode(relayB, OUTPUT);
    digitalWrite(relayA, LOW);
    digitalWrite(relayB, LOW);

    pinMode(AC_Sense, INPUT);
    delay(500);

    switchState = digitalRead(AC_Sense);
    if(switchState) {
      Serial.printf("switch state is now: A\n");
      LoadStatus = ON;
    }else {
      Serial.printf("switch state is now: B\n");
      LoadStatus = OFF;
    }
      
    for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
    }

    // Init Switch State
    AC_Sense_readState = digitalRead(AC_Sense);
    toggleLoad(&LoadStatus, &switchState);
    toggleLoad(&LoadStatus, &switchState);
    toggleLoad(&LoadStatus, &switchState);
    toggleLoad(&LoadStatus, &switchState);
    delay(200);
    if(AC_Sense_readState != digitalRead(AC_Sense))
      toggleLoad(&LoadStatus, &switchState);
    
    WiFiMulti.addAP("WifiNaam", "qawsEDRF");
    WiFiMulti.addAP("MIND-WIFI", "87654321");

    Serial.printf("Connecting Wifi...\n");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.printf(".");
        delay(100);
    }
    
    Serial.printf("\n");
    Serial.printf("WiFi connected\n");
    Serial.printf("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("\n");

    //webSocket.begin("tornado-server.local/", 8880, "/web_ws");
    //webSocket.begin("192.168.1.90", 8880, "/web_ws");
    webSocket.beginSSL("192.168.1.90", 8880, "/web_ws");
    
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent); 
}

void loop() {
    // Web Socket loop
    webSocket.loop();

    AC_Sense_readState = digitalRead(AC_Sense);
    if((AC_Sense_readState != LoadStatus)) {
      // toggle switch state
      if(switchState == STATE_A) {
        switchState = STATE_B;
        Serial.printf("switch is now: B\n");
      }else {
        switchState = STATE_A;
        Serial.printf("switch is now: A\n");
      }      
          
      LoadStatus = AC_Sense_readState;
      if(LoadStatus)
        Serial.printf("Load ON\n");
      else
        Serial.printf("Load OFF\n");
   }
}

