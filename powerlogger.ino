#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include <Adafruit_INA260.h>
#include <Fonts/FreeMono9pt7b.h>

Adafruit_INA260 ina260 = Adafruit_INA260();

const int chipSelect = 3;

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     LED_BUILTIN // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



void setup() {
  Serial.begin(9600);

/////Setup for the RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

/////Setup for the display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

/////Setup for the SD card
    Serial.print("Initializing SD card...");
 
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  //boot message
  display.clearDisplay();
  delay(100);

  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Alec Hudson");
  display.display();
  delay(1000);
  display.println("This is a test");
  display.display();

    if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");
  
  delay(2000); // Pause for 2 seconds

}

void loop() {
   DateTime now = rtc.now();
    
/////Writing the data to SD card
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(", ");

    dataFile.print(ina260.readCurrent());
    dataFile.print(" mA");
    dataFile.print(", ");
    dataFile.print(ina260.readBusVoltage());
    dataFile.print(" mV");
    dataFile.print(", ");
    dataFile.print(ina260.readPower());
    dataFile.print(" mW");
    
    dataFile.println();
    dataFile.close();
 
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

/////Power Measurement
  display.clearDisplay();
  display.setCursor(0,10);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont(&FreeMono9pt7b);
  
  display.print(ina260.readCurrent());
  display.println(" mA");

  display.print(ina260.readBusVoltage());
  display.println(" mV");

  display.print(ina260.readPower());
  display.println(" mW");

  display.display();
  display.println();


  Serial.print("Current: ");
  Serial.print(ina260.readCurrent());
  Serial.println(" mA");

  Serial.print("Bus Voltage: ");
  Serial.print(ina260.readBusVoltage());
  Serial.println(" mV");
  Serial.print("Power: ");
  Serial.print(ina260.readPower());
  Serial.println(" mW");

/////RTC Time and temp

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

  delay(1000);
  
}
