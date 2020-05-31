//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


#define LED_BUILTIN        2     // GPIO-02


AsyncWebServer server(80);

const char* ssid = "AandeWiFi";
const char* password = "3741TS12tl";
uint32_t    blinkyTimer;

static const char indexPage[] =
R"(
<!DOCTYPE html>
<html charset="UTF-8">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">  
    <script src="/index.js"></script>
    <link rel="stylesheet" type="text/css" href="/index.css">
    <title>ESP32_Framework</title>
  </head>
  <body>
    <font face="Arial">
    <!-- <div class="dev-header"> -->
    <div class="header">
      <h1>
        <span id="sysName">ESP32 Simple Webserver</span> &nbsp; &nbsp; &nbsp;
        <span id="devName"    style='font-size: small;'>devName</span> &nbsp;
        <span id="devVersion" style='font-size: small;'>[devVersion]</span>
        <span id='theTime' class='nav-item nav-clock'>theTime</span>
      </h1>
    </div>
    </font>
    <div id="displayMainPage"      style="display:none">
      <div class="nav-container">
        <div class='nav-left'>
          <a id='saveMsg' class='nav-item tabButton' style="background: lightblue;">opslaan</a>
          <a id='M_FSexplorer'    class='nav-img'>
                      <img src='/FSexplorer.png' alt='FSexplorer'></a>
          <a id='Settings'      class='nav-img'>
                      <img src='/settings.png' alt='Settings'></a>
        </div>
      </div>
      <br/>
      <div id="mainPage">
        <div id="waiting">Wait! loading page .....</div>
      </div>
    </div>

    <div id="displaySettingsPage"  style="display:none">
      <div class="nav-container">
        <div class='nav-left'>
          <a id='back' class='nav-item tabButton' style="background: lightblue;">Terug</a>
          <a id='saveSettings' class='nav-item tabButton' style="background: lightblue;">opslaan</a>
          <a id='S_FSexplorer' class='nav-img'>
                      <img src='/FSexplorer.png' alt='FSexplorer'></a>
        </div>
      </div>
      <br/>
      <div id="settingsPage"></div>
    </div>
  
    <!-- KEEP THIS --->

    <!-- Pin to bottom right corner -->
    <div class="bottom right-0">Simple Webserver</div>

    <!-- Pin to bottom left corner -->
    <div id="message" class="bottom left-0">-</div>

    <script>
      console.log("now continue with the bootstrapMain");
      window.onload=bootsTrapMain;
      var countdown = setInterval(function() {
          window.location.assign("/")
      }, 3000);

    </script>

  </body>

</html>
)";

void sendIndexPage(AsyncWebServerRequest *request)
{
  Serial.print("sendIndexPage() .. From: ");
  Serial.println(request->client()->remoteIP());

  request->send(200, "text/html", indexPage);

} // sendIndexPage()

void restAPI(AsyncWebServerRequest *request, uint8_t *bodyData, size_t bodyLen) 
{
  Serial.print("restAPI() .. [");
  Serial.print(request->url().c_str());
  Serial.print("] From: ");
  Serial.println(request->client()->remoteIP());
  delay(300);
  
  request->send(200, "text/plain", "OK");
  
} // restAPI()

void notFound(AsyncWebServerRequest *request) {
  Serial.print("notFound() .. [");
  Serial.print(request->url().c_str());
  Serial.print("] From: ");
  Serial.println(request->client()->remoteIP());
  delay(200);
  
  request->send(404, "text/plain", "Not found");

} // notFound()

void setup() 
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  //================ SPIFFS ===========================================
  // https://github.com/espressif/arduino-esp32/issues/3000
  //if (SPIFFS.begin(false,"/spiffs",30)) 
  if (SPIFFS.begin()) 
  {
    Serial.println(F("SPIFFS Mount succesfull\r"));
  } else { 
    Serial.println(F("SPIFFS Mount failed\r"));   // Serious problem with SPIFFS 
  }

    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    sendIndexPage(request);
  });
  
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    restAPI(request, 0, 0);
  });
  
  server.serveStatic("/index.css",      SPIFFS, "/index.css");
  server.serveStatic("/index.js",       SPIFFS, "/index.js");
  server.serveStatic("/FSexplorer.png", SPIFFS, "/FSexplorer.png");
  server.serveStatic("/settings.png",   SPIFFS, "/settings.png");
  server.serveStatic("/flavicon.ico",   SPIFFS, "/flavicon.ico");

  server.onNotFound(notFound);

  server.begin();
    
} // setup()

void loop() 
{
  if (millis() > blinkyTimer)
  {
    blinkyTimer = millis() + 2000;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

} // loop()
