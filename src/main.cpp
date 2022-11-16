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

// File paths to save input values permanently
const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *ipPath = "/ip.txt";
const char *gatewayPath = "/gateway.txt";

IPAddress localIP;
// IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
// IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

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
AsyncWebHandler *handlerrr;

// Initialize SPIFFS
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- frite failed");
  }
}

// Initialize WiFi
bool initWiFi()
{
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip);
  localGateway.fromString(gateway);

  Serial.println(subnet);


  if (!WiFi.config(localIP, localGateway, subnet)){
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
    String buttons = "";
    buttons += sieci;

    buttons += "</br></br>";

    return buttons;
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

void setup()
{
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
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

  if (initWiFi())
  {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html", false, processor); });
    server.serveStatic("/", SPIFFS, "/");

    // Route to set GPIO state to HIGH
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      digitalWrite(ledPin, HIGH);
      request->send(SPIFFS, "/index.html", "text/html", false, processor); });

    // Route to set GPIO state to LOW
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
              {
      digitalWrite(ledPin, LOW);
      request->send(SPIFFS, "/index.html", "text/html", false, processor); });
    server.begin();
  }
  else
  {
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
        // Serial.println("in the loop");
        // Print SSID and RSSI for each network found

        String temp = WiFi.SSID(i);

        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(temp);
        sieci += "<input type=\"radio\"  name=\"ssid\" value=\"" + temp + "\">";
        sieci += "<label for=\"html\">" + temp + "</label><br>";
      }
    }
    Serial.println(sieci);

    dnsServer.start(53, "*", WiFi.softAPIP());

    handlerrr = new CaptiveRequestHandler();

    server.addHandler(handlerrr).setFilter(ON_AP_FILTER); // only when requested from AP

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
      server.removeHandler(handlerrr);
      dnsServer.stop();
    }
  }
}