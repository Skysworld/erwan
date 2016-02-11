
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";
String httpRequest;  
int timer1=0;

/////////////////////
// Pin Definitions //
/////////////////////
int PIN_BTN_GREEN=12;
int LED1 = 13; //16 
int LED2 = 15; //15
int LED3 = 4;//12
int LED4 = 2;
int LED5 = 0;

//Autres
const char* ssid     = "ProjetNETGEAR2";
const char* password = "projetIRIS";
uint8_t MAC_array[6];
char MAC_char[18];
String commande;

//Definition des methodes //evite les probleme d'appel
void configuration();


ESP8266WebServer  server(80);



void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED:
        {
            Serial.printf("Disconnected!\n");
        }
        break;
          
        case WStype_CONNECTED:
                {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
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
            Serial.printf("TEXT DU SERVEUR %s\n", payload);
            
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

void handleRoot() {
  String s ;
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s +="<h1>Config du SSID et Mot de passe </h1>";
  s += "<form action='http://192.168.4.1/submit' method='POST'> SSID:<br><input type=textt name=ssid value='ssid'>";
  s += "<br> Mot de passe: <br> <input type=text name=mdp value='mdp'>";
  s+= "<br><br><input type='submit' value='Comfirme'></form> ";
  s += "</html>\n";
  server.send(200, "text/html", s);  
}


void handleSubmit()
{
  String ssid = server.arg("ssid");
  String mdp =  server.arg("mdp");
  char *cssid = (char*)ssid.c_str();
  char*cmdp =(char*)mdp.c_str();
  String s ;
  int timer=0;
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s +="<h1>Connection a la box avec vos identifiant</h1>";
   server.send(200, "text/html", s);
   WiFi.mode(WIFI_STA);
  WiFi.begin(cssid,cmdp);
  while ((WiFi.status() != WL_CONNECTED && timer < 10)) {
    delay(200);
    digitalWrite(LED1,LOW);
    delay(500);
    digitalWrite(LED1,HIGH);
    delay(200);
    Serial.println("Connecting ...");
    timer++;
  }//fin du while
  if(timer == 10)
  {
    Serial.println("echec retour mode config");
    configuration();
  }
  else
  {
  String c ;
  c += "<!DOCTYPE HTML>\r\n<html>\r\n";
  c +="<h1>Vous etes connecter</h1>";
  server.send(200, "text/html", c);
  IPAddress myAddress = WiFi.localIP();
  Serial.println(myAddress);
  digitalWrite(LED1,LOW);
  webSocket.begin("192.168.0.36", 9300);
  webSocket.onEvent(webSocketEvent);
  timer1 =100;
  }
}

void setup() 
{
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
  Serial.begin(115200);
  pinMode(PIN_BTN_GREEN, INPUT);
  pinMode(LED1, OUTPUT);  
  WiFi.mode(WIFI_STA);
}


void configuration()
{
  digitalWrite(LED1,HIGH);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  
  char AP_NameString[] = "ESP8266TEST";
  
  WiFi.softAP(AP_NameString, WiFiAPPSK);
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();
  
 
}

void loop() 
{
  int val=0;
  delay(1000);
  Serial.println(timer1);
  
  val = digitalRead(PIN_BTN_GREEN);
  if (val == HIGH)
  {
    timer1++;
  }    
  if(timer1 == 3)
  {
    configuration();
  }
  if(timer1 < 99)
    {
      server.handleClient();
      Serial.println("AP");
    } 
  if(timer1==100)
  {
    
    webSocket.loop();
  }
  if(timer1>103)
  {
    timer1=0;
  }
}

 
