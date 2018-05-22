#include <Adafruit_FONA.h>
#include <SoftwareSerial.h>

#define FONA_RX (2)
#define FONA_TX (3)
#define FONA_RST (4)

#define BAUD_FONA (4800)
#define BAUD_USB (115200)

char reply_buffer[255];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

float latitude;
float longitude;

void setup()
{
    while (!Serial); // Wait for serial port to open
    Serial.begin(BAUD_USB);

    fonaSerial->begin(BAUD_FONA);
    if (!fona.begin(*fonaSerial))
    {
        Serial.println(F("Couldn't find FONA"));
        while(1); // Hang if no FONA
    }

    // Save for later?
    char imei[16] = {0};
    fona.getIMEI(imei);

    // Enable GPRS
    fona.setGPRSNetworkSettings(F("services.telenor.se"));
    while (!fona.enableGPRS(true))
    {
        Serial.println(F("Failed to turn on GPRS"));
        fona.enableGPRS(false);
        delay(3000);
    }
}

void loop()
{
    if(!fona.getGSMLoc(&latitude, &longitude))
    {
        Serial.println(F("Failed to fetch location"));
    }
    else
    {
        Serial.print(F("Location is "));
        Serial.print(latitude, 12);
        Serial.print(F(" , "));
        Serial.print(longitude, 12);
        Serial.print(F("\n"));
    }
    delay(5000);
}