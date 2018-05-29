#include <Adafruit_FONA.h>
#include <SoftwareSerial.h>
#include <firebase.h>
// #include "api.key" // Macro called API_KEY

#define FONA_RX (2)
#define FONA_TX (3)
#define FONA_RST (4)

#define BAUD_FONA (4800)
#define BAUD_USB (115200)

char reply_buffer[255];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

typedef struct
{
    // Should add some type of uncertainty here
    float lat;
    float lng;
    float spd;
    float hdg;
    float alt;
} pos_t;

pos_t pos = {0};

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
        Serial.println(F("Failed to turn on GPRS, retry in 3s"));
        fona.enableGPRS(false);
        delay(3000);
    }
    fona.setHTTPSRedirect(true);

    // Enable GPS
    while (!fona.enableGPS(true))
    {
        Serial.println(F("Failed to turn on GPS, retry in 3s"));
        fona.enableGPS(false);
        delay(3000);
    }
}

void loop()
{

    boolean gps_success = fona.getGPS(&pos.lat, &pos.lng, NULL, NULL, NULL);

    // False if GPRS location hasn't been fetched first
    if (gps_success == false)
    {
        Serial.println(F("Failed to fetch GPS coordinates"));

        int gps_status = fona.GPSstatus();
        Serial.print(F("GPS status: "));
        Serial.print(gps_status);

        fona.getGSMLoc(&pos.lat, &pos.lng);
    }

    if !(fona.HTTP_POST_start(url, F("application/json"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length))
    {

    }

    Serial.print(F("Location is "));
    Serial.print(pos.lat, 12);
    Serial.print(F(" , "));
    Serial.print(pos.lng, 12);
    Serial.print(F("\n"));

    delay(5000);
}