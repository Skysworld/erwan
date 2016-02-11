#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

const char* ssid     = "ProjetNETGEAR2";
const char* password = "projetIRIS";
uint8_t MAC_array[6];
char MAC_char[18];
String commande;

int LED1 = 13; //16
int LED2 = 15; //15
int LED3 = 4;//12
int LED4 = 2;
int LED5 = 0;
#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {


    switch(type) {
        case WStype_DISCONNECTED:
        {
            USE_SERIAL.printf("Disconnected!\n");
        }
        break;
          
        case WStype_CONNECTED:
                {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
                //Envoie de l'adresse mac 0
                 WiFi.macAddress(MAC_array);
                 int i = 0;
                for (i; i < sizeof(MAC_array)-1; ++i){
                sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);}
                 sprintf(MAC_char,"%s%02x",MAC_char,MAC_array[i]);
                 //Envoie de l'adresse mac 
					       webSocket.sendTXT(MAC_char);
                delay(1000);
                webSocket.sendTXT(MAC_char);            
                }
                break;
        case WStype_TEXT:
            USE_SERIAL.printf("TEXT DU SERVEUR %s\n", payload);
            
            if (payload[0]=='1')
              digitalWrite(LED1, LOW);
            else
              digitalWrite(LED1, HIGH);
             if (payload[1]=='1')
              digitalWrite(LED2, LOW);
            else
              digitalWrite(LED2, HIGH);
            if (payload[2]=='1')
              digitalWrite(LED3, LOW);
            else
              digitalWrite(LED3, HIGH);
             if (payload[3]=='1')
              digitalWrite(LED4, LOW);
            else
              digitalWrite(LED4, HIGH);
             if(payload[4]=='1')
              digitalWrite(LED5, LOW);
             else
              digitalWrite(LED5, HIGH);
            break;
    }

}

void setup() {
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  webSocket.begin("192.168.0.36", 9300);
  webSocket.onEvent(webSocketEvent);

}

void loop() {
    webSocket.loop();
}
