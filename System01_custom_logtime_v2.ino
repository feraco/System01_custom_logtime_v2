#include <Adafruit_HMC5883_U.h>

#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

#include <Adafruit_BME280.h>
#include <Adafruit_HMC5883_U.h>

// Sensors
Adafruit_BME280 bme; // I2C for barometric sensor
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// SD card
const int SD_CS = 10;

File myFile;
char filename[17]; //make sure to define correct # of characters

// General Variables
float mag_field = 0.0;

boolean done = false;
unsigned long last_time = 0;

int log_time = 60; // default is 1 minute



void setup() {
  Serial.begin(115200);
  pinMode(SD_CS, OUTPUT); //Adafruit SD shields and modules: pin 10

  do {
    Serial.println("Requesting time from Pi");
    sync_now();
    delay(2000);

  }  while (timeStatus() == timeNotSet);

  Serial.println("Time was set correctly");
  digitalClockDisplay();

  boolean ok = false;
  do {
    Serial.println("Requesting Log time from Pi");
    ok = getLogTime();
    delay(2000);
  } while (!ok);

  /* check on all components */
  if (!SD.begin(SD_CS)) {
    Serial.println(F("initialization failed!"));
    while (1);
  }

  Serial.println(F("SD card found and initialized"));

  if (!bme.begin()) {
    Serial.println(F("BME not found, check wiring!"));
    while (1);
  }

  if (!mag.begin()) {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1);
  }

  last_time = now();
  Serial.println(F("Good to go"));
}

void loop() {

  sensors_event_t event;
  mag.getEvent(&event);

  mag_field = sqrt(
                (event.magnetic.x * event.magnetic.x)
                + (event.magnetic.y * event.magnetic.y)
                + (event.magnetic.z * event.magnetic.z)
              ); // in uTesla
  mag_field = mag_field / 100; // in gauss

  if (timeStatus() != timeNotSet) {
    if (isAM()) {
      if (hour() == 12 && !done) {
        myFile.close();
        sprintf(filename, "wwhs%02d%02d%04d.txt", day(), month(), year());
        myFile = SD.open(filename, FILE_WRITE);
        Log2Card();
        done = true;
      } else {
        done = false;
      }
    }

    if (now() - last_time >= log_time) {
      last_time = now();
      digitalClockDisplay();
      Serial.print("Now:"); Serial.println(now());
      Serial.print("last:"); Serial.println(last_time);
      sprintf(filename, "wwhs%02d%02d%04d.txt", day(), month(), year());
      myFile = SD.open(filename, FILE_WRITE);
      Log2Card();
    }
  }
  update_log_time();
}

// write to file
void Log2Card() {
  String data = "{\"log\":\"";
  // Add date
  data += String(day());
  data += "/";
  data += String(month());
  data += "/";
  data += String(year());
  data += ",";

  // Add time
  data += String(hour());
  data += ":";
  data += String(minute());
  data += ":";
  data += String(second());
  data += ",";

  // Add Magnetic Field
  data += mag_field;
  data += ",";

  // Add temperature
  data += String(bme.readTemperature());
  data += ",";

  // Add pressure
  data += String(bme.readPressure() / 100.0F);
  data += ",";

  // Add humidity
  data += String(bme.readHumidity());

  data += "\"}";

  Serial.println(data); // Send data to Raspberry Pi
  myFile.println(data); // Log to file

  myFile.flush();
  myFile.close();
}


String getDateTime() {
  unsigned long timeout = millis();
  String ret = "##/##/####,##:##:##.###";
  Serial.println("#whatTime");
  while (!Serial.available()) {
    if (millis() - timeout > 2000) {
      break;
    }
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'T') {
      ret = Serial.readStringUntil('\n');
      ret.trim();
      break;
    }
  }
  return ret;
}

unsigned long getEpoch() {
  unsigned long timeout = millis();
  unsigned long ret = 0;
  Serial.println("#whatEpoch");
  while (!Serial.available()) {
    if (millis() - timeout > 2000) {
      break;
    }
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'E') {
      ret = Serial.parseInt();
      break;
    }
  }
  return ret;
}

void sync_now() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  pctime = getEpoch();
  Serial.println(pctime);
  if ( pctime >= DEFAULT_TIME) {
    setTime(pctime);
  }
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}


void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

boolean update_log_time() {
  boolean result = false;
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '$') {
      int val = Serial.parseInt();
      if (val > 0) {
        if (val != log_time) {
          log_time = val;
          Serial.print("Log time: ");
          Serial.print(log_time);
          Serial.println(" seconds");
          Serial.println("{\"log_time\": \"done\"}");
        }
        result = true;
      }
    }
  }
  return result;
}

boolean getLogTime() {
  unsigned long timeout = millis();
  boolean ret = false;
  Serial.println("#whatLogTime");
  while (!Serial.available()) {
    if (millis() - timeout > 2000) {
      return ret;
    }
  }
  ret = update_log_time();
  return ret;
}

