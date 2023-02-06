/*------------------------------------------------------------------------------
  14-23 April 2021
  Author: Abdul-Khaliq Nathekar
  Platforms: ESP8266
  Language: C++/Arduino
  File: Coingecko-Json-Serial-OLED-GitHub.ino
  ------------------------------------------------------------------------------
  Description:
  Code demonstrating parsing json data from Coingecko.com API to fetch
  the current price of various crypto coins.
  ------------------------------------------------------------------------------
  Change log:

  V1.00
  First Release
  1. Secure-connect and get data from Coingecko.com API
  2. Parse Json test text string and send to serial monitor

  V1.01
  1. Incoporate Time & Date retrieval from NTP server

  V2.00
  1. Added 128x64 I2C OLED
  2. Send required data to OLED

  V2.01
  1. Format placement of data along with appropriate typeface
  2. Adde price change in 24hrs

  V2.02
  1. Centre price on OLED
  2. Removed PageBuffers for U8g2 printing
  
   V2.03 - 16 Oct 2021
  1. Updated Coingecko Fingerprint

  ------------------------------------------------------------------------------*/


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//Insert your local WiFi network details here.
const char* ssid     = "SSID";
const char* password = "WiFi_Password";

WiFiUDP ntpUDP;
// You may change the NTP server address to one of your choosing below.
NTPClient timeClient(ntpUDP, "0.uk.pool.ntp.org", 3600, 60000);

// Modify your API URL/string to retrieve coin prices of your choosing.  Check out: https://www.coingecko.com/en/api#explore-api
#define CG_URL "https://api.coingecko.com/api/v3/simple/price?ids=verasity%2Cbitcoin%2Ccardano%2Cenjincoin%2Cripple%2Clivepeer&vs_currencies=gbp%2Cusd&include_last_updated_at=true&include_24hr_change=true"

// SSL SHA-1 Fingerprint may be retrieved from your browser. e.g. for Chrome, Right-click the Padlock inthe address bar, select Certificate.
// Expand Details, scroll down to Fingerprints, copy the 20-byte SHA-1 key and paste below

// If data retrieval fails, you may need to update the site Fingerprint.
// Head over to https://www.grc.com/fingerprints.htm and enter api.coingecko.com into the URL entry box and click on "Fingerprint site."
const char *fingerprint  = "A1 63 09 9D 7B D6 C1 7F 47 B4 D1 83 0E 7D 36 A9 18 5C 0D 40";

String payload = "{}";


void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(10);

  u8g2.begin();
  u8g2.enableUTF8Print();

  //  Serial.println();
  //  Serial.println();
  //  Serial.print("Connecting to ");
  //  Serial.println(ssid);

  WiFi.begin(ssid, password);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB08_tf); // choose a suitable font

  u8g2.setCursor(1, 15);
  u8g2.print("Connecting to ");
  u8g2.setCursor(1, 25);
  u8g2.print(ssid);
  u8g2.setCursor(1, 35);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //    Serial.print(".");
    u8g2.print(".");
    u8g2.sendBuffer();
  }

  timeClient.begin();
  //  timeClient.setTimeOffset(1);

  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.localIP());
}


void loop()
{
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED)
  {
    // Instanciate Secure HTTP communication
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    HTTPClient http;  //Object of class HTTPClient

    delay(200);

    // Get NTP Time and print to Serial monitor
    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }
    Serial.println();
    Serial.print("Time     - ");
    Serial.println(timeClient.getFormattedTime());

    // Display Time on OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_tf);
    u8g2.setCursor(23, 40);
    u8g2.print(timeClient.getFormattedTime());
    u8g2.sendBuffer();

    delay (3000);
    payload = "{}";

    // Send Coingecko query URL
    http.begin(client, CG_URL);

    //    Serial.println();
    //    Serial.print("Coingecko URL - ");
    //    Serial.println(CG_URL);

    // Get response code in order to decide if API determined request as valid.
    int httpCode = http.GET();
    //    Serial.print("Site Response : ");
    //    Serial.println(httpCode);

    // Check the returning code
    if (httpCode > 0) {
      // Get the request response payload (Price data)
      payload = http.getString();

      // Parsing
      //      Serial.print("Json String - ");
      //      Serial.println(payload);

      // If you add or remove any parameters from the URL query, head over to the ArduinoJson Assistant to determine the size of the buffer.
      // https://arduinojson.org/v6/assistant/
      // Ensure V6 of the ArduinoJson library is installled.
      StaticJsonDocument<800> doc;

      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        delay(5000);
        return;
      }

      // Parameters
      // The ArduinoJson assistant will generate your readable parameters.  Paste them here and remove any that you don't need.
      JsonObject ripple = doc["ripple"];
      double ripple_gbp = ripple["gbp"]; // 0.949746
      double ripple_gbp_24h_change = ripple["gbp_24h_change"]; // -3.554174217019491
      double ripple_usd = ripple["usd"]; // 1.32
      //      double ripple_usd_24h_change = ripple["usd_24h_change"]; // -3.6005246399970887
      //      long ripple_last_updated_at = ripple["last_updated_at"]; // 1619044798

      JsonObject cardano = doc["cardano"];
      double cardano_gbp = cardano["gbp"]; // 0.876531
      double cardano_gbp_24h_change = cardano["gbp_24h_change"]; // -3.4183415615054225
      double cardano_usd = cardano["usd"]; // 1.22
      //      double cardano_usd_24h_change = cardano["usd_24h_change"]; // -3.4647572636282584
      //      long cardano_last_updated_at = cardano["last_updated_at"]; // 1619044755

      JsonObject bitcoin = doc["bitcoin"];
      double bitcoin_gbp = bitcoin["gbp"]; // 39068
      double bitcoin_gbp_24h_change = bitcoin["gbp_24h_change"]; // -3.7228749254037727
      double bitcoin_usd = bitcoin["usd"]; // 54422
      double bitcoin_usd_24h_change = bitcoin["usd_24h_change"]; // -3.778129914824945
      long bitcoin_last_updated_at = bitcoin["last_updated_at"]; // 1619044823

      JsonObject verasity = doc["verasity"];
      double verasity_gbp = verasity["gbp"]; // 0.0266054
      double verasity_gbp_24h_change = verasity["gbp_24h_change"]; // -5.726089874347541
      double verasity_usd = verasity["usd"]; // 0.03706132
      //      double verasity_usd_24h_change = verasity["usd_24h_change"]; // -5.771396507108699
      //      long verasity_last_updated_at = verasity["last_updated_at"]; // 1619044699

      JsonObject enjincoin = doc["enjincoin"];
      double enjincoin_gbp = enjincoin["gbp"]; // 1.83
      double enjincoin_gbp_24h_change = enjincoin["gbp_24h_change"]; // -0.06487692122149383
      double enjincoin_usd = enjincoin["usd"]; // 2.54
      //      double enjincoin_usd_24h_change = enjincoin["usd_24h_change"]; // -0.11290424834960369
      //      long enjincoin_last_updated_at = enjincoin["last_updated_at"]; // 1619044789

      JsonObject livepeer = doc["livepeer"];
      double livepeer_gbp = livepeer["gbp"]; // 22.34
      double livepeer_gbp_24h_change = livepeer["gbp_24h_change"]; // 5.70284426500335
      double livepeer_usd = livepeer["usd"]; // 31.12
      //      double livepeer_usd_24h_change = livepeer["usd_24h_change"]; // 5.642179703851018
      //      long livepeer_last_updated_at = livepeer["last_updated_at"]; // 1619044791

      long epoch = doc["bitcoin"]["last_updated_at"];
      // epoch = (epoch / 86400) + 25569;  Date

      // Convert Bitcoin "updated Epoch Time" into readable format
      epoch = epoch + 3600  ; //ADD 1 Hours (For GMT+1)
      // print the hour, minute and second:
      int hr    = (epoch  % 86400L) / 3600;
      int minit = (epoch % 3600) / 60;
      int sec   = (epoch % 60);

      // Send HH:MM:SS time to serial monitor
      Serial.print("Updated  - ");
      if (hr < 10) {
        Serial.print("0");
      }
      Serial.print(hr);
      Serial.print(":");
      if (minit < 10) {
        Serial.print("0");
      }
      Serial.print(minit);
      Serial.print(":");
      if (sec < 10) {
        Serial.print("0");
      }
      Serial.println(sec);
      Serial.println();

      // Edit below your coin holdings
      double holdings_cardano  =  1000.555;
      double holdings_bitcoin  =  2;
      double holdings_verasity =  2000.5443;
      double holdings_enjin    =  1000.0324;
      double holdings_ripple   =  5000;
      double holdings_livepeer =  10;

      // Send information from Json payload of your choosing to the serial monitor
      Serial.print("Cardano  - £");
      Serial.print(cardano_gbp, 4);
      Serial.print("     $");
      Serial.print(cardano_usd);
      Serial.print("       £");
      Serial.println(cardano_gbp * holdings_cardano);
      Serial.println();

      Serial.print("Bitcoin  - £");
      Serial.print(bitcoin_gbp);
      Serial.print("   $");
      Serial.print(bitcoin_usd);
      Serial.print("   £");
      Serial.println(bitcoin_gbp * holdings_bitcoin);
      Serial.println();

      Serial.print("Verasity - £");
      Serial.print(verasity_gbp, 4);
      Serial.print("     $");
      Serial.print(verasity_usd);
      Serial.print("       £");
      Serial.println(verasity_gbp * holdings_verasity);
      Serial.println();

      Serial.print("Enjin    - £");
      Serial.print(enjincoin_gbp, 4);
      Serial.print("     $");
      Serial.print(enjincoin_usd);
      Serial.print("       £");
      Serial.println(enjincoin_gbp * holdings_enjin);
      Serial.println();

      Serial.print("XRP      - £");
      Serial.print(ripple_gbp, 4);
      Serial.print("     $");
      Serial.print(ripple_usd);
      Serial.print("       £");
      Serial.println(ripple_gbp * holdings_ripple);
      Serial.println();

      Serial.print("Livepeer - £");
      Serial.print(livepeer_gbp);
      Serial.print("      $");
      Serial.print(livepeer_usd);
      Serial.print("      £");
      Serial.println(livepeer_usd * holdings_livepeer);

      Serial.println();
      Serial.print("Holdings - £");

      double holdings =
        (cardano_gbp   * holdings_cardano) +
        (bitcoin_gbp   * holdings_bitcoin) +
        (verasity_gbp  * holdings_verasity) +
        (enjincoin_gbp * holdings_enjin) +
        (ripple_gbp    * holdings_ripple) +
        (livepeer_gbp  * holdings_livepeer);

      Serial.println(holdings);
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();

      // Send coin prices to OLED.
      // Parameters are a. Scrolling string, b. Currency Price, c. Decimal Places, d. Currency Price Change, e. Currency Symbol
      draw("CARDANO - ADA",    cardano_gbp,   4,  cardano_gbp_24h_change,   "£");
      draw("BITCOIN - BTC",    bitcoin_gbp,   0,  bitcoin_gbp_24h_change,   "£");
      draw("BITCOIN - BTC",    bitcoin_usd,   0,  bitcoin_usd_24h_change,   "$");
      draw("VERASITY - VRA",   verasity_gbp,  5,  verasity_gbp_24h_change,  "£");
      draw("ENJIN COIN - ENJ", enjincoin_gbp, 2,  enjincoin_gbp_24h_change, "£");
      draw("RIPPLE - XRP",     ripple_gbp,    4,  ripple_gbp_24h_change,    "£");
      draw("LIVEPEER - LPT",   livepeer_gbp,  2,  livepeer_gbp_24h_change,  "£");
      draw("HOLDINGS  ",       holdings,      2,  0,                        "£");
    }
    http.end();   //Close connection
  }
}

// The helper functions below are direct copies from the u8g2 example set with some modification
/*
  Draw a string with specified pixel offset.
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];  // should for screen with up to 256 pixel width
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset) / 8;
    dx = offset + char_offset * 8;
    if ( char_offset >= u8g2.getDisplayWidth() / 8 )
      return;
    visible = u8g2.getDisplayWidth() / 8 - char_offset + 1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset * 8 - dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return; // nothing visible
    dx = offset - char_offset * 8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth() / 8 + 1 )
      visible = u8g2.getDisplayWidth() / 8 + 1;
    strncpy(buf, s + char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
}

void draw(char *s, double coinprice, int prec, double change, String currency)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);

  // Calculate for centring of Price
  char pricelen[10];
  dtostrf(coinprice, 4, prec, pricelen);
  int xPos = 64 - (((strlen(pricelen)) * 11) / 2);

  for (;;)
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB18_tf);
    u8g2.setCursor(xPos - 11, 26);
    u8g2.print(currency);
    u8g2.print(coinprice, prec);

    if (change != 0) {
      u8g2.setFont(u8g2_font_helvB10_tf);
      u8g2.setCursor(38, 43);
      if (change > 0) {
        u8g2.print("+");
      }
      u8g2.print(change, 2);
      u8g2.print("%");
    }

    drawScrollString(offset, s);
    u8g2.sendBuffer();
    delay(10);
    offset += 2;
    if ( offset > len * 8 + 1 )
      break;
  }
}
