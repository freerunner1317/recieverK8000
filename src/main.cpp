#include <RadioLib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SX1278 radio = new Module(10, 2, 9, 3);
void testdrawchar(byte * byteArray, size_t length);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1278 radio = RadioShield.ModuleA;

void setup() {
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();

  display.clearDisplay();

  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  if (radio.setBandwidth(500.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    while (true);
  }

  // set spreading factor to 10
  if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    while (true);
  }
  if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION ){
    Serial.println(F("Selected CRC is invalid for this module!"));
    while (true);
  }
  if (radio.setSyncWord(0xDF) != RADIOLIB_ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    while (true);
  }
}

void loop() {
  //Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  String str;
  //int state = radio.receive(str);

  // you can also receive data as byte array
  
  byte byteArr[3];
  String mess = "";
  int state = radio.receive(byteArr, 3);
  

  if (1) {
    // packet was successfully received
    //Serial.println(F("success!"));

    // print the data of the packet
    //Serial.print(F("[SX1278] Data:\t\t\t"));
    //Serial.println(str);
    //Serial.print((char)0xF3);
    mess = "";
    mess += (char)0xF3;
    for (byte i = 0; i < 3; i++) { // выводим элементы массива      
      // char text[2] = "";
      // sprintf(text, "%02X", byteArr[i]);
      // Serial.print(text);
      // //Serial.print((char)byteArr[i]);
      mess += (char)byteArr[i];
    }
    //Serial.println(); 
    mess += (char)0xFC;
    Serial.print(mess);
    //Serial.print((char)0xFC);
    //Serial.println(); 
    //testdrawchar(byteArr, 3);
    // // print the RSSI (Received Signal Strength Indicator)
    // // of the last received packet
    // Serial.print(F("[SX1278] RSSI:\t\t\t"));
    // Serial.print(radio.getRSSI());
    // Serial.println(F(" dBm"));

    // // print the SNR (Signal-to-Noise Ratio)
    // // of the last received packet
    // Serial.print(F("[SX1278] SNR:\t\t\t"));
    // Serial.print(radio.getSNR());
    // Serial.println(F(" dB"));

    // // print frequency error
    // // of the last received packet
    // Serial.print(F("[SX1278] Frequency error:\t"));
    // Serial.print(radio.getFrequencyError());
    // Serial.println(F(" Hz"));

  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    //Serial.println(F("timeout!"));

  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
}

void testdrawchar(byte * byteArray, size_t length) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(uint8_t i = 0; i < length; i++) {
    char text[2] = "";
    sprintf(text, "%02X", byteArray[i]);
    display.write(text);
  }display.println();
  // display.print(F("RSSI: "));
  // display.print(radio.getRSSI());
  // display.println(F(" dBm"));

  // display.print(F("SNR: "));
  // display.print(radio.getSNR());
  // display.println(F(" dB"));

  // display.print(F("Fq err: "));
  // display.print(radio.getFrequencyError());
  // display.println(F(" Hz"));

  display.display();
}


