# OctoPrint mini Status for M5Stack
Very simple example for a M5Stack hosted OctoPrint status viewer

### What does it do?
It's showing
* the current Printer-Status
* Tool Temp
* Bed Temp
* A kind off Progress Bar if a print is running

![Demo Picture](http://lexat.net:2904/lexat/M5Stack-Octoprint-View/raw/master/img/demo.png)

## Getting Started
* If you want to use the graphics, copy content of SD-Card folder to the root of your SD-Card
For my example I used an SD Updater + Menu
If you dont want to use this just comment at the beginning
```
/*
 #include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater
 #include <M5TreeView.h>         // https://github.com/lovyan03/M5Stack_TreeView
 #include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/
*/
```
```
and in the Setup
/*
  if(digitalRead(BUTTON_A_PIN) == 0) {
     Serial.println("Will Load menu binary");
     updateFromFS(SD);
     ESP.restart();
  }
*/
```
What you need is:
* OctoPrintAPI -> https://github.com/chunkysteveo/OctoPrintAPI
* depending on "ArduinoJSON" i took the 5.13.5 Version cause >= 6 will not work with the API lib