#include <OctoPrintAPI.h> // https://github.com/chunkysteveo/OctoPrintAPI
#include <M5Stack.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater/
#include <M5TreeView.h>         // https://github.com/lovyan03/M5Stack_TreeView
#include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/
#define GFXFF 1
#define IrPin 13
#define PLUS_ADDR 0x62
#define Sdebug false

#define M5STACK_FIRE_NEO_NUM_LEDS 12
#define M5STACK_FIRE_NEO_DATA_PIN 15

const char* ssid = "xxxxx";
const char* password = "xxxxx";

int light = 0;

WiFiClient client;

// what happends when buttons where pressed
void buttons_code() {
  if (M5.BtnA.wasPressed()) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  if (M5.BtnB.wasPressed()) {

  }
  if (M5.BtnC.wasPressed()) {
    setLight();
  }
}

// You only need to set one of the of follwowing:
IPAddress ip(192, 168, 178, 90);                         // Your IP address of your OctoPrint server (inernal or external)
// char* octoprint_host = "octoprint.example.com";  // Or your hostname. Comment out one or the other.

const int octoprint_httpPort = 80;  //If you are connecting through a router this will work, but you need a random port forwarded to the OctoPrint server from your router. Enter that port here if you are external
String octoprint_apikey = "xxxxx"; //See top of file or GIT Readme about getting API key

String printerOperational;
String printerPaused;
String printerPrinting;
String printerReady;
String printerText;
String printerHotend;
String printerTarget;
String payload;
// Use one of the following:
OctoprintApi api(client, ip, octoprint_httpPort, octoprint_apikey);               //If using IP address
// OctoprintApi api(client, octoprint_host, octoprint_httpPort, octoprint_apikey);//If using hostname. Comment out one or the other.

unsigned long api_mtbs = 30000; //mean time between api requests (60 seconds)
unsigned long api_lasttime = 0;   //last time api request has been done

void setLight() {
  if (light < 4) {
    light++;
  } else {
    light = 0;
  }
  switch (light) {
    case 0:
      M5.Lcd.setBrightness(10);
      // statements
      break;
    case 1:
      M5.Lcd.setBrightness(100);
      // statements
      break;
    case 2:
      M5.Lcd.setBrightness(150);
      // statements
      break;
    case 3:
      M5.Lcd.setBrightness(255);
      // statements
      break;
    case 4:
      M5.Lcd.setBrightness(0);
      // statements
      break;
    default:
      M5.Lcd.setBrightness(10);
      // statements
  }
}

void setup () {
  M5.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawJpgFile(SD, "/ops/m5back.jpg", 0, 0, 320, 240);
  M5.Lcd.setCursor(1, 1);
  M5.Lcd.setTextColor(BLACK, GREEN);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Octoprint M5Stack");
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP32 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //if you get here you have connected to the WiFi
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  buttons_code();
  M5.update();
  if (millis() - api_lasttime > api_mtbs || api_lasttime == 0) { //Check if time has expired to go check OctoPrint
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      if (api.getOctoprintVersion()) {
        if (Sdebug) {
          Serial.println("---------Version---------");
          Serial.print("Octoprint API: ");
          Serial.println(api.octoprintVer.octoprintApi);
          Serial.print("Octoprint Server: ");
          Serial.println(api.octoprintVer.octoprintServer);
          Serial.println("------------------------");
        }
      }
      Serial.println();
      if (api.getPrinterStatistics()) {
        if (Sdebug) {
          Serial.println("---------States---------");
          Serial.print("Printer Current State: ");
          Serial.println(api.printerStats.printerState);
        }
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(1, 1);
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setTextSize(3);
        M5.Lcd.printf("Octoprint M5Stack");
        M5.Lcd.setCursor(1, 30);
        M5.Lcd.setTextColor(RED, BLACK);
        M5.Lcd.setTextSize(2);
        String sstate = String(api.printerStats.printerState);
        if (sstate != "Printing") {
          M5.Lcd.drawJpgFile(SD, "/ops/m5back.jpg", 0, 0, 320, 240);
          M5.Lcd.println("----------Status--------");
          M5.Lcd.println("Status: " + sstate);
          M5.Lcd.println("------Termperatures-----");
          M5.Lcd.println("Printer Temp - Tool0 (C):");
          String ttemp = String(api.printerStats.printerTool0TempActual) + " C";
          String btemp = String(api.printerStats.printerBedTempActual) + " C";
          M5.Lcd.println("Tool: " + ttemp);
          M5.Lcd.println(" ");
          M5.Lcd.println("Printer Temp - Bed (C):   ");
          M5.Lcd.println("Bed: " + btemp);
          M5.Lcd.println(" ");
          M5.Lcd.println("Overall");
        } else {
          M5.Lcd.drawJpgFile(SD, "/ops/m5backmenu.jpg", 0, 0, 320, 240);
          //M5.Lcd.fillRoundRect(1, 30, 65, 65, 2, WHITE);
          M5.Lcd.drawJpgFile(SD, "/ops/nozzle.jpg", 15, 30, 65, 65);
          //M5.Lcd.drawJpgFile(SD, "/ops/fan.jpg", 125, 30, 65, 65);
          M5.Lcd.drawJpgFile(SD, "/ops/bed.jpg", 235, 30, 65, 65);          
          M5.Lcd.setCursor(1, 100);
          String ttemp = String(api.printerStats.printerTool0TempActual) + " C";
          String btemp = String(api.printerStats.printerBedTempActual) + " C";
          M5.Lcd.println(ttemp);
          M5.Lcd.setCursor(220, 100);
          M5.Lcd.println(btemp);
          M5.Lcd.setCursor(1, 120);
          M5.Lcd.println("Status: " + sstate);
          M5.Lcd.setCursor(1, 170);
          M5.Lcd.println("Overall");
          double percent = 0.0;
          if (api.getPrintJob()) {
            percent = api.printJob.progressCompletion;
          } else {
            percent = 100;
          }
          M5.Lcd.setCursor(1, 170);
          String perc = String(percent);
          M5.Lcd.println("Overall " + perc + "%");
          //320 * 240
          M5.Lcd.fillRect(3, 195, 314, 16, WHITE);
          M5.Lcd.fillRect(4, 196, 312, 14, BLACK);
          int barl = (312 / 100) * percent;
          M5.Lcd.fillRect(4, 196, barl, 14, GREEN); 
        }
        // 0xC618
        M5.Lcd.setTextColor(BLACK);        
        M5.Lcd.drawString("SD Menu", 57, 225, GFXFF);
        M5.Lcd.drawString("N/A", 160, 225, GFXFF);
        M5.Lcd.drawString("Light", 267, 225, GFXFF);
        M5.Lcd.setCursor(80,6);
        M5.Lcd.print("Octoprint Status");
        if (Sdebug) {
          Serial.print("Printer State - closedOrError:  ");
          Serial.println(api.printerStats.printerStateclosedOrError);
          Serial.print("Printer State - error:  ");
          Serial.println(api.printerStats.printerStateerror);
          Serial.print("Printer State - operational:  ");
          Serial.println(api.printerStats.printerStateoperational);
          Serial.print("Printer State - paused:  ");
          Serial.println(api.printerStats.printerStatepaused);
          Serial.print("Printer State - printing:  ");
          Serial.println(api.printerStats.printerStatePrinting);
          Serial.print("Printer State - ready:  ");
          Serial.println(api.printerStats.printerStateready);
          Serial.print("Printer State - sdReady:  ");
          Serial.println(api.printerStats.printerStatesdReady);
          Serial.println("------------------------");
          Serial.println();
          Serial.println("------Termperatures-----");
          Serial.print("Printer Temp - Tool0 (°C):  ");
          Serial.println(api.printerStats.printerTool0TempActual);
          Serial.print("Printer State - Tool1 (°C):  ");
          Serial.println(api.printerStats.printerTool1TempActual);
          Serial.print("Printer State - Bed (°C):  ");
          Serial.println(api.printerStats.printerBedTempActual);
          Serial.println("------------------------");
          if (api.getPrintJob()) {
            Serial.println("-------Job (Progress information)-------");
            Serial.print("progressCompletion (%) float:\t\t");
            Serial.println(api.printJob.progressCompletion);
            Serial.print("progressFilepos (bytes) long:\t\t");
            Serial.println(api.printJob.progressFilepos);
            Serial.print("progressPrintTime (sec) long:\t\t");
            Serial.println(api.printJob.progressPrintTime);
            Serial.print("progressPrintTimeLeft (sec) long:\t");
            Serial.println(api.printJob.progressPrintTimeLeft);
            Serial.println("----------------------------------------");
            Serial.println();
          }
        }
      }
    }
    api_lasttime = millis();  //Set api_lasttime to current milliseconds run
  }
}
