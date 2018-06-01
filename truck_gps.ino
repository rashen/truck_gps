#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "secrets.h" /* stores BACKEND_URL and API_KEY */

#define FONA_RX (2)
#define FONA_TX (3)
#define FONA_RST (4)

#define BAUD_FONA (4800)
#define BAUD_USB (115200)

#define PREC_NONE (0)
#define PREC_GSM  (1)
#define PREC_GPS  (2)

char reply_buffer[255];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

typedef struct
{
    float lat;
    float lng;
    float speed;
    float heading;
    float altitude;
    uint8_t precision = PREC_NONE;
} pos_t;

static pos_t gs_pos;

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

    // Enable GPRS
    fona.setGPRSNetworkSettings(F("services.telenor.se"));
    while (!fona.enableGPRS(true))
    {
        Serial.println(F("Failed to turn on GPRS, retry in 3s"));
        fona.enableGPRS(false);
        delay(3000);
    }
    fona.setHTTPSRedirect(true);
}

static bool m_post_coordinates(void)
{
    static char api_key[] = API_KEY;
    static char url[] = BACKEND_URL;

    static char lat_c[16] = {0};
    static char lng_c[16] = {0};
    static char data[100] = {0};

    uint16_t status_code;
    uint16_t len;

    dtostrf(gs_pos.lat, 4, 10, lat_c);
    dtostrf(gs_pos.lng, 4, 10, lng_c);

    snprintf(data, sizeof(data), "{\"key\": \"%s\", \"lat\": %s, \"lng\": %s, \"prc\": %d}", api_key, lat_c, lng_c, gs_pos.precision);

    Serial.println(data);

    if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &status_code, &len))
    {
        Serial.println(F("Post failed"));
        return false;
    }

    // Read response buffer
    while (len > 0)
    {
        while (fona.available())
        {
            char c = fona.read();
            Serial.write(c);
            len--;
        }
    }
    fona.HTTP_POST_end();
    return true;
}

void loop()
{
    int gps_status = fona.GPSstatus();
    Serial.print(F("GPS status: "));
    Serial.print(gps_status);

    if (fona.getGPS(&gs_pos.lat, &gs_pos.lng, NULL, NULL, NULL))
    {
        Serial.println("Fetched GPS location successfully");
        gs_pos.precision = PREC_GPS;
    }
    else
    {
        Serial.println(F("Failed to fetch GPS coordinates, fetching GSM location instead"));
        fona.getGSMLoc(&gs_pos.lat, &gs_pos.lng);
        gs_pos.precision = PREC_GSM;
    }

    if (!m_post_coordinates())
    {
        Serial.println(F("Failed to post coordinates to backend"));
    }

    Serial.print(F("Location is "));
    Serial.print(gs_pos.lat, 12);
    Serial.print(F(" , "));
    Serial.print(gs_pos.lng, 12);
    Serial.print(F("\n"));

    delay(30000);
}