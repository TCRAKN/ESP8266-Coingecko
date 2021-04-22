# ESP8266-Coingecko
Arduino ESP8266 Crypto Ticker using Coingecko.com API and ArduinoJson.

A work in progress in order to migrate to a scrolling colour WS2812b 32x8 LED matrix panel.

Incorporating WeMos D1 ESP8266 moduleCoingecko.com API, ArduinoJson and I2C OLED screen to display cryptocurrencies of choice (through code modification).

The current version displays price (in GBP), % change over 24 hrs.  Text positioning isn't the most elegant.  It doesn't need to be as it's a stepping-stone to a 32x8 matrix panel.

No API Key is required for connecting to Coingecko.  There are however request limits you'll need to observe.

You can modify the sketch quite easily to retrieve data from Coinmarketcap.com etc...
