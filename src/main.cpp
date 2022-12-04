/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-wi-fi-manager-asyncwebserver/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
//#include "initializeWifi.h"
#include "spiffsActions.h"
#include "enginesControl.h"
#include "variables.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dnsServer;
bool clientConnected = false;

// Search for parameter in HTTP POST request
const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "pass";
const char *PARAM_INPUT_3 = "ip";
const char *PARAM_INPUT_4 = "gateway";

// Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

IPAddress localIP;
// IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
// IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

AsyncWebHandler *requestHandler;

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

// Set LED GPIO
const int ledPin = 2;
// Stores LED state

String ledState;
int n;
String sieci = "";
int counter = 0;

// Initialize SPIFFS
void initSPIFFS();

// Initialize WiFi
bool initWiFi()
{
  if (ssid == "" || ip == "")
  {
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip);
  localGateway.fromString(gateway);

  Serial.println(subnet);

  if (!WiFi.config(localIP, localGateway, subnet))
  {
    Serial.println("STA Failed to configure");
    return false;
  }

  const char *ssidEr = ssid.c_str();
  const char *passEr = pass.c_str();

  WiFi.begin(ssidEr, passEr);
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {

      Serial.println("Failed to connect.");
      counter = 0;
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  MDNS.begin("esp");
  return true;
}

String processor(const String &var)
{
  if (var == "BUTTONPLACEHOLDER")
  {
    return sieci;
  }
  return String();
}

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/wifimanager.html", "text/html", false, processor);
    Serial.println("async capture");
    counter++;
  }
};

// methids from robot

// websocket to control the robot manually, get info wchich direction is selected
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;

    Serial.println((char *)data);

    if (strcmp((char *)data, "toggle") == 0)
    {
      ledState = !ledState;
      //  notifyClients();
    }
    if (strcmp((char *)data, "forward") == 0)
    {
      forwardBool = 1;
      changed = 1;
    }
    if (strcmp((char *)data, "back") == 0)
    {
      backBool = 1;
      changed = 1;
    }
    if (strcmp((char *)data, "left") == 0)
    {
      leftBool = 1;
      changed = 1;
    }
    if (strcmp((char *)data, "right") == 0)
    {
      rightBool = 1;
      changed = 1;
    }
    if (strcmp((char *)data, "stop") == 0)
    {
      Serial.println("STOPPP");
      changed = 1;
      stopBool = 1;
    }

    if (strcmp((char *)data, "ctrl") == 0)
    {
      controll != controll;
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void manualControl()
{
  if (forwardBool == 1)
  {
    forward();
    forwardBool = 0;
  }
  else if (backBool == 1)
  {
    back();
    backBool = 0;
  }
  else if (rightBool == 1)
  {
    right();
    rightBool = 0;
  }
  else if (leftBool == 1)
  {
    left();
    leftBool = 0;
  }
  else if (bRightBool == 1)
  {
    rightB();
    bRightBool = 0;
  }
  else if (backLeftBool == 1)
  {
    leftB();
    backLeftBool = 0;
  }
  else

      if (stopBool == 1)
  {
    stopBool = 0;
    stop();
  }
}

void colisionEvader(){
  
}

void setup()
{

  // setup engine pins
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);

  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();

  // Set GPIO 2 as an OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile(SPIFFS, gatewayPath);
  /*   Serial.println(ssid);
    Serial.println(pass);
    Serial.println(ip);
    Serial.println(gateway); */

  const char *ssidChar = ssid.c_str();
  const char *passChar = pass.c_str();
  const char *ipChar = ip.c_str();
  const char *gatewayChar = gateway.c_str();
  // initWiFiii_init();

  if (initWiFi())
  {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html", false); });
    server.serveStatic("/", SPIFFS, "/");

    // init event listener
    events.onConnect([](AsyncEventSourceClient *client)
                     {
                     if (client->lastId())
                     {
                       Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
                     }
                     // send event with message "hello!", id current millis
                     // and set reconnect delay to 1 second
                     client->send("hello!", NULL, millis(), 10000); });
    server.addHandler(&events);

    server.begin();
  }
  else
  {
    counter = 0;
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == -2)
    {
      Serial.println("SCAN FAILED if this happens often then needs to be handled");
    }
    else if (n == 0)
    {
      Serial.println("no networks found");
    }
    else
    {
      Serial.print(n);
      Serial.println(" networks found");
      sieci == "";
      for (int i = 0; i < n; ++i)
      {

        // Print SSID and RSSI for each network found

        String currentWifiId = WiFi.SSID(i);

        /*
          //Print all avaliable networks in console
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(currentWifiId); */
        sieci += "<input type=\"radio\"  name=\"ssid\" value=\"" + currentWifiId + "\">";
        sieci += "<label for=\"html\">" + currentWifiId + "</label><br>";
      }
    }
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      request->send(SPIFFS, "/wifimanager.html", "text/html", false, processor);
         Serial.println("Client Connected");  
         clientConnected=true; });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
              {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(SPIFFS, ipPath, ip.c_str());
          }
          // HTTP POST gateway value
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("Gateway set to: ");
            Serial.println(gateway);
            // Write file to save value
            writeFile(SPIFFS, gatewayPath, gateway.c_str());
          }
                  clientConnected=true;

          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip+"\n or http://192.168.0.120 \n or try dns address: http://esp.local");
      delay(5000);
      ESP.restart(); });

    dnsServer.start(53, "*", WiFi.softAPIP());

    requestHandler = new CaptiveRequestHandler();

    server.addHandler(requestHandler).setFilter(ON_AP_FILTER); // only when requested from AP

    server.begin();
  }
}

void loop()
{
  if (WiFi.getMode() != 1)
  {
    if (counter <= 3)
    {

      dnsServer.processNextRequest();
    }
    else
    {
      server.removeHandler(requestHandler);
      dnsServer.stop();
    }
  }
  else
  {
    ws.cleanupClients();

  }
}