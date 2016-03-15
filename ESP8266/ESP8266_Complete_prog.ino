#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiPassword[] = "123456789"; //Password of the ESP locale WIFI 
char AP_NameString[] =  "ESP826612" ; // Name of the ESPs SSID in local WIFI

/////////////////////
// Pin Definitions //
/////////////////////
int PIN_BTN =16;
int PIN_LED_BLUE = 13;
int PIN_LED_RED = 15;
int PIN_LED_GREEN = 12;
int LED1= 4; 
int LED2 = 5;
int LED3 = 0;
int LED4 = 2;
int LED5 = 14;

/////////////////////
//      Other      //
/////////////////////
uint8_t MAC_array[6];
char MAC_char[18];
char Request_MAC[19];
bool reboot_flag = false;
int timer_push_button =0;
enum connection {NO_CONNECTED,CONNECTED,CONFIGURATION};
connection state;


//////////////////////////
//Definition of methods//
/////////////////////////
void configuration();

ESP8266WebServer  server(80);

//
//
//
//
void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED:
        {
            digitalWrite(PIN_LED_RED, LOW);
        }
        break;
          
        case WStype_CONNECTED:
                {
                Serial.printf("Connected to WS Server",  payload);
                 WiFi.macAddress(MAC_array);//Get the MAC Address of the ESP 
                 int i = 0;
                for (i; i < sizeof(MAC_array)-1; ++i){
                sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);}
                 sprintf(MAC_char,"%s%02x",MAC_char,MAC_array[i]);//Make the MAC Address from basic string to format "XX:XX:XX"
                 webSocket.sendTXT(MAC_char);//Send a message "MacAddress" for the server to associated with WS's ID
                digitalWrite(PIN_LED_RED,HIGH);//Turn on the RED LED 
                Request_MAC[0]='#';
                strcat(Request_MAC,MAC_char);
                webSocket.sendTXT(Request_MAC);
                memset(MAC_char, 0 ,18);//Clean the MAC_char array for avoiding concatenation of 2 message
                memset(Request_MAC, 0 ,18);
                }
                break;
        case WStype_TEXT:
            Serial.printf("Message received %s", payload);
            
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
//This function show the config page of the SSID and Password 
//you must connect to 192.168.1.4 to see this page
//
//TODO : make the UI adaptive
void handleRoot() {
  String page ;
  page += "<!DOCTYPE HTML>\r\n<html>\r\n";
  page +="<h1>Config du SSID et Mot de passe </h1>";
  page += "<form action='http://192.168.4.1/submit' method='POST'> SSID:<br><input type=textt name=ssid value='ssid'>";
  page += "<br> Mot de passe: <br> <input type=text name=mdp value='mdp'>";
  page += "<br><br><input type='submit' value='Comfirme'></form> ";
  page += "</html>\n";
  server.send(200, "text/html", page);  
}

//HandleSubmit let the ESP to manage the SSID and password given by the user
//to connect the ESP to WIFI of the user
//A timer of 20 seconde is use to detect and connect the ESP
//If the ESP failed to connect to WIFI , the function configuration() is called 
void handleSubmit()
{
  String ssid = server.arg("ssid");
  String password = server.arg("mdp");
  char *cssid = (char*)ssid.c_str();
  char *cmdp =(char*)password.c_str();
  String page;
  int timer =0;
  page += "<!DOCTYPE HTML>\r\n<html>\r\n";
  page +="<h1>Tentative de connexion avec la box...</h1>";
   server.send(200, "text/html", page);
   WiFi.mode(WIFI_STA);  //The ESP switch on Station mode
  WiFi.begin(cssid,cmdp); //Try to connect ESP with the SSID and Password provide
  while ((WiFi.status() != WL_CONNECTED && timer < 10))
  {
    delay(200);
    digitalWrite(PIN_LED_BLUE,LOW);
    delay(500);
    digitalWrite(PIN_LED_BLUE,HIGH);
    delay(200);
    timer++;
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    delay(500);
  reboot_flag==true;
  digitalWrite(PIN_LED_BLUE,LOW);
  digitalWrite(PIN_LED_GREEN,HIGH);
  webSocket.begin("172.17.50.156", 9300);
  webSocket.onEvent(webSocketEvent);
  Serial.println(WiFi.localIP());
  state= CONNECTED;
  }
  else
  {
    state=CONFIGURATION;
  }
}

//Basic function of the Arduino program
//Call when the ESP start
//Set up the I/O of the ESP
//Try to connect to WIFI which is stored in the flash memory (previous assigned SSID/Password)
void setup() 
{
  reboot_flag = false;
  state=NO_CONNECTED;
  pinMode(LED1, OUTPUT);//Set the ouput GPIO for the relay
  pinMode(LED2, OUTPUT);//Set the ouput GPIO for the relay
  pinMode(LED2, OUTPUT);//Set the ouput GPIO for the relay
  pinMode(LED3, OUTPUT);//Set the ouput GPIO for the relay
  pinMode(LED4, OUTPUT);//Set the ouput GPIO for the relay
  pinMode(LED5, OUTPUT);//Set the ouput GPIO for the relay
  digitalWrite(LED1, LOW);//Set the ouput GPIO for the relay at LOW statement 
  digitalWrite(LED2, LOW);//Set the ouput GPIO for the relay at LOW statement
  digitalWrite(LED3, LOW);//Set the ouput GPIO for the relay at LOW statement
  digitalWrite(LED4, LOW);//Set the ouput GPIO for the relay at LOW statement
  digitalWrite(LED5, LOW);//Set the ouput GPIO for the relay at LOW statement
  Serial.begin(115200);//Set the serial baud rate for displaying
  pinMode(PIN_LED_BLUE, OUTPUT);//Set the GPIO for LED 
  pinMode(PIN_LED_GREEN, OUTPUT);//Set the GPIO for LED
  pinMode(PIN_LED_RED, OUTPUT);//Set the GPIO for LED
  WiFi.mode(WIFI_STA);//Put the ESP in Station mode
  delay(500);
  WiFi.begin("","");//Charge the previous SSID/Pass stored in flash memory
  delay(1000); 
}

//This function enter the mode configuration of the ESP
//Use the previous function to make the user see the pages
//on 192.168.1.4
//
void configuration()
{
   WiFi.disconnect();// Disconnect the ESP from the WIFI
   delay(500);
   if (WiFi.status() != WL_CONNECTED) //Enter the condition if the ESP isnot connected to WIFI 
   {
    digitalWrite(PIN_LED_GREEN, LOW);
   }
  delay(500);
  WiFi.mode(WIFI_AP);//Set the WIFI to AP mode
  delay(500);
  WiFi.softAP(AP_NameString, WiFiPassword);//Set the WIFI access Point for the configuration
  delay(500);
  server.on("/", handleRoot);//Binding for the function 
  server.on("/submit", handleSubmit);//Binding
  server.begin(); //Start the server of configuration for connecting the ESP to the WIFI BOX 
  
}
//
//
//
//

void loop() 
{
  delay(100);
  
  if (digitalRead(PIN_BTN) == LOW) // Enter this conditiion if the button is pressed 
  {
    timer_push_button++;
  }
  else //If the button isnot pressed
  {
    timer_push_button=0; 
  }
  if(timer_push_button == 30) //Enter the configuration mode
  {
    state = CONFIGURATION;
    timer_push_button=0; 
  }
  
 switch (state){
  
  case CONNECTED:
    if (reboot_flag == false)
    {
      Serial.println("dans la boucle if");
      reboot_flag = true;
      digitalWrite(PIN_LED_GREEN, HIGH);
      webSocket.begin("172.17.50.156", 9300); // Init the websocket IP address and port
      webSocket.onEvent(webSocketEvent); 
    }
    else // Normal state of the ESP
    {
      webSocket.loop();//Start the WEBSOCKET client loop
    }
    break;
    
   case CONFIGURATION:
     timer_push_button=0;
     digitalWrite(PIN_LED_BLUE, HIGH); // Turn on the BLUE LED on the ESP
     digitalWrite(PIN_LED_RED, LOW);
     configuration();//Launch the function of configuration
     server.handleClient();//Start the server of configuration loop     
     break;
     
   case NO_CONNECTED:
    if(WiFi.status() == WL_CONNECTED)
     {
    state=CONNECTED;    
    }
    digitalWrite(PIN_LED_GREEN, LOW);// Turn off the GREEN LED
    break;

 }  
}

 
