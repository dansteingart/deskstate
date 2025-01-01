#include <Adafruit_CircuitPlayground.h>
#include <ArduinoJson.h>
#include "Adafruit_TinyUSB.h"


// Enable USB CDC (Serial) and WebUSB
Adafruit_USBD_WebUSB WebUSBSerial; // WebUSB for direct communication

// Buffer for accumulating data
#define BUFFER_SIZE 256
char inputBuffer[BUFFER_SIZE];
size_t bufferIndex = 0;


void setup() {
    TinyUSBDevice.setID( 0x5150, 0x0812 );
    TinyUSBDevice.setManufacturerDescriptor( "Pithy Industries" );
    TinyUSBDevice.setProductDescriptor( "Desk Light" );


  if (!TinyUSBDevice.isInitialized()) { TinyUSBDevice.begin(0);}
  Serial.begin(115200);


  CircuitPlayground.begin();
  WebUSBSerial.begin();


  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1);

  Serial.println("Ready to receive JSON commands via Serial or WebUSB...");
}

void handleInput(String input, bool isUSB) {
  StaticJsonDocument<200> doc;
  Serial.print(input);
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    if (isUSB) {
      WebUSBSerial.print("Error parsing JSON: ");
      WebUSBSerial.println(error.c_str());
    } else 
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
    
    return;
  }

  // Set brightness if provided
  if (doc.containsKey("brightness")) {
    int brightness = doc["brightness"];
    brightness = constrain(brightness, 0, 255);
    CircuitPlayground.setBrightness(brightness);

    if (isUSB) {
      WebUSBSerial.print("Set brightness to ");
      WebUSBSerial.println(brightness);
    } 

    Serial.print("Set brightness to ");
    Serial.println(brightness);
    
  }

  // Set LED color
  if (doc.containsKey("led") && doc.containsKey("color")) {
    int led = doc["led"];
    int r = doc["color"]["r"];
    int g = doc["color"]["g"];
    int b = doc["color"]["b"];

    if (led >= 0 && led < 10) {
      CircuitPlayground.setPixelColor(led, r, g, b);

      if (isUSB) {
        WebUSBSerial.print("Set LED ");
        WebUSBSerial.print(led);
        WebUSBSerial.println(" to color.");
      } 

        Serial.print("Set LED ");
        Serial.print(led);
        serializeJson(doc, Serial);
        Serial.println(" to color.");
      
    } else {
      if (isUSB) {
        WebUSBSerial.println("Invalid LED index.");
      } 
        Serial.println("Invalid LED index.");
      
    }
  }
}

void loop() {
  // Handle Serial communication
  TinyUSBDevice.task();
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    handleInput(input, false); // Handle Serial input
  }

  // Handle WebUSB communication
  if (WebUSBSerial.available()) {

    while (WebUSBSerial.available()) {
      char c = WebUSBSerial.read();

      // Accumulate data into the buffer
      if (bufferIndex < BUFFER_SIZE - 1) {
        inputBuffer[bufferIndex++] = c;
      }

      // Check for end of message (newline character)
      if (c == '\n') {
        inputBuffer[bufferIndex] = '\0'; // Null-terminate the string
        handleInput((char*)inputBuffer, true); // Handle WebUSB input
        bufferIndex = 0;                // Reset the buffer
      }
    }


    
  }
}