#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "secrets.h" // Stores BACKEND_URL and API_KEY

#define FONA_RX (2)
#define FONA_TX (3)
#define FONA_RST (4)

#define BAUD_FONA (4800)
#define BAUD_USB (115200)

#define PREC_NONE (0)
#define PREC_GSM  (1)
#define PREC_GPS  (2)

#define ITERATION_SLEEP_TIME_MS (10000)
#define DELAY_3_MS (3000)

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
    Serial.begin(BAUD_USB);

    fonaSerial->begin(BAUD_FONA);
    while (!fona.begin(*fonaSerial))
    {
        Serial.println(F("ERROR: No FONA board could be found, retry in 3s."));
        delay(DELAY_3_MS);
    }
    Serial.println(F("SUCCESS: FONA serial connection established."));

    fona.setGPRSNetworkSettings(F("services.telenor.se"));
    fona.setHTTPSRedirect(true);

    while (!fona.enableGPRS(true))
    {
        Serial.println(F("ERROR: Failed to turn on GPRS, retry in 3s."));
        fona.enableGPRS(false);
        delay(DELAY_3_MS);
    }
    Serial.println(F("SUCCESS: GPRS enabled."));

    while (!fona.enableGPS(true))
    {
        Serial.println(F("ERROR: Failed to turn on GPS, retry in 3s."));
        fona.enableGPS(false);
        delay(DELAY_3_MS);
    }
    Serial.println(F("SUCCESS: GPS enabled."));
}

static bool m_post_coordinates(void)
{
    static char api_key[] = API_KEY;
    static char url[] = BACKEND_URL;

    static char lat_c[16] = {0};
    static char lng_c[16] = {0};
    static char data[160] = {0};

    uint16_t status_code;
    uint16_t len;

    // Convert float coordinate to string
    dtostrf(gs_pos.lat, 4, 10, lat_c);
    dtostrf(gs_pos.lng, 4, 10, lng_c);

    snprintf(data, sizeof(data), "{\"key\": \"%s\", \"pos\": { \"lat\": %s, \"lng\": %s, \"prc\": %d} }", api_key, lat_c, lng_c, gs_pos.precision);

    bool error_code = fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &status_code, &len);

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

    return error_code;
}

void loop()
{
    int gps_status = fona.GPSstatus();
    Serial.print(F("INFO: GPS status: "));
    Serial.print(gps_status);

    if (fona.getGPS(&gs_pos.lat, &gs_pos.lng, NULL, NULL, NULL))
    {
        Serial.println("SUCCESS: Fetched GPS location successfully.");
        gs_pos.precision = PREC_GPS;
    }
    else
    {
        Serial.println(F("ERROR: Failed to fetch GPS coordinates, using GSM location instead."));
        fona.getGSMLoc(&gs_pos.lat, &gs_pos.lng);
        gs_pos.precision = PREC_GSM;
    }

    if (!m_post_coordinates())
    {
        Serial.println(F("ERROR: Failed to post coordinates to backend."));
    }
    else
    {
        Serial.println(F("SUCCESS: Coordinated posted succesfully."));
    }

    Serial.print(F("INFO: Location is "));
    Serial.print(gs_pos.lat, 12);
    Serial.print(F(", "));
    Serial.print(gs_pos.lng, 12);
    Serial.print(F(". Precision: "));
    Serial.print(gs_pos.precision);
    Serial.print(F("."));

    Serial.println(F(""));
    Serial.print(F("INFO: Sleeping for "));
    Serial.print(ITERATION_SLEEP_TIME_MS);
    Serial.print(F(" ms."));
    delay(ITERATION_SLEEP_TIME_MS);
}