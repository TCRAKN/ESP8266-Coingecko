# ESP8266-Coingecko
Arduino ESP8266 Crypto Ticker using Coingecko.com API and ArduinoJson.

A work in progress in order to migrate to a scrolling colour WS2812b 32x8 LED matrix panel.

Incorporating WeMos D1 ESP8266 module, Coingecko.com API, ArduinoJson and I2C OLED screen to display cryptocurrencies of choice (through code modification).

The current version displays price and % change over 24 hrs.  

No API Key is required for connecting to Coingecko.  There are however request limits you'll need to observe.

You can modify the sketch quite easily to retrieve data from Coinmarketcap.com etc...

I2C 128x64 OLED wired to D1 & D2 of the WeMos/Node MCU boards.  D1 - SCL, D2 - SDA.  Wire OLED Power to 3.3V on ESP board and GND to GND.

Please make sure you have installed the latest Arduino IDE, Libraries and Boards.

You'll need these libraries:
ESP8266WiFi.h
ESP8266HTTPClient.h
ArduinoJson.h - V7
NTPClient.h
WiFiUdp.h
Wire.h
U8g2lib.h

Please consider contributing to Benoît's efforts by buying his invaluable book - Mastering ArduinoJson 7 at
https://arduinojson.org/book/
