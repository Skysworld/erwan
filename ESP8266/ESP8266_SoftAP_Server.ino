#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>


//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";
String httpRequest;  
int timer1=0;

int PIN_BTN_GREEN=12;

/////////////////////
// Pin Definitions //
/////////////////////
const int BLUE_LED = 13; // Thing's onboard, blue LED
void configuration();


ESP8266WebServer  server(80);


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
    digitalWrite(BLUE_LED,LOW);
    delay(500);
    digitalWrite(BLUE_LED,HIGH);
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
  digitalWrite(BLUE_LED,LOW);
  timer1 =100;
  }
}

void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_BTN_GREEN, INPUT);
  pinMode(BLUE_LED, OUTPUT);  
  WiFi.mode(WIFI_STA);
}


void configuration()
{
  digitalWrite(BLUE_LED,HIGH);
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
  else if(timer1 < 99)
    {
      server.handleClient();
      Serial.println("AP");
    } 
  if(timer1==100)
  {
    Serial.println("Websocket");
  }
}

 
