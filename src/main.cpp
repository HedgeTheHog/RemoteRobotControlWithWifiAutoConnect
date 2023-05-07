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
#include <DNSServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "spiffsActions.h"
#include "enginesControl.h"
#include "variables.h"
#include "eventGetter.h"
#include "autoNavigation.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebHandler *requestHandler;
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
DNSServer dnsServer;

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
String networksToInjectToHTML = "";


IPAddress localIP;
// IPAddress localIP(192, 168, 1, 200); // hardcoded

//IPAddress localGateway;
IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 255, 0); //hardcoded

int amountOfNetworks;
int requestsCounter = 0; //needed for auto connect on mobile, for some reason 3 requests are needed

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
  //localGateway.fromString(gateway);

  Serial.println(subnet);

  if (!WiFi.config(localIP, localGateway, subnet))
  {
    Serial.println("STA Failed to configure");
    return false;
  }

  const char *ssidEr = ssid.c_str();
  const char *passEr = pass.c_str();

  WiFi.begin(ssidEr, passEr);
  Serial.print("Connecting to WiFi...");

  int waitCounter=0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
    waitCounter++;
    if (waitCounter>4)
    {
      Serial.println("Failed to connect.");
      requestsCounter = 0;
     WiFi.disconnect(); 
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
    return networksToInjectToHTML;
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
    requestsCounter++;
  }
};

// methods from robot

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

int autoPilot(int travelDirections)
{
    if ((millis() - lastTime) > timerDelay)
    {
      getSensorReadings();
      String directionReadings =String(sonarOneReading) + ":" +String(sonarTwoReading);
      events.send(directionReadings.c_str(), "distance", millis());  //because of this I failed to move this function to other file
      travelDirections = getTravelDirection();
      lastTime = millis();
    }

  return travelDirections;
}
//
void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  initSPIFFS();
  setupEngine();

  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile(SPIFFS, gatewayPath);
  /*   Serial.println(ssid);
    Serial.println(pass);
    Serial.println(ip);
    Serial.println(gateway); */

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

    initWebSocket();
    server.addHandler(&events);
    server.begin();
  }
  else
  {
    requestsCounter = 0;
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    amountOfNetworks = WiFi.scanNetworks();
    Serial.println("scan done");
    if (amountOfNetworks == -2)
    {
      Serial.println("SCAN FAILED if this happens often then needs to be handled");
    }
    else if (amountOfNetworks == 0)
    {
      Serial.println("no networks found");
    }
    else
    {
      Serial.print(amountOfNetworks);
      Serial.println(" networks found");
      for (int i = 0; i < amountOfNetworks; ++i)
      {

        // Print SSID and RSSI for each network found
        String currentWifiId = WiFi.SSID(i);

          Serial.print(i + 1);
          Serial.print(": ");
          Serial.println(currentWifiId); 
        networksToInjectToHTML += "<input type=\"radio\"  name=\"ssid\" value=\"" + currentWifiId + "\"><label for=\"html\">" + currentWifiId + "</label><br>";
      }
    }
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      request->send(SPIFFS, "/wifimanager.html", "text/html", false, processor);
         Serial.println("Client Connected"); });

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
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip+"\n or http://192.168.0.120 \n or try dns address: http://esp.local");
      delay(5000);
      ESP.restart(); 
      });

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
    if (requestsCounter <= 3)
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

    if (userSelectedDirection == 99)
    {
      Serial.print("changing control to: ");
      remoteControl = !remoteControl;
      Serial.println(remoteControl);
      userSelectedDirection = 100;
    }
    if (remoteControl == 0)
    {
      userSelectedDirection = manualControl(userSelectedDirection);
    }
    else
    {
      automaticallySelectedDirection = autoPilot(automaticallySelectedDirection);
      automaticallySelectedDirection = manualControl(automaticallySelectedDirection);
    }
  }
}