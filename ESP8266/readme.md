# ESP8266 Notes

[ESP8266 NodeMCU CP2102 ESP-12E Development Board](https://www.amazon.com/dp/B081CSJV2V) $5.43 on Amazon

NodeMCU Pinout | Select Board
---|---
![](esp8266-breakout-schematic.jpg)|![](select.png)

## WiFi Connection

```cpp
#include <ESP8266WiFi.h>

const char* ssid = "my_ssid";
const char* password = "my_secret_password";

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}
```

## HTTP GET
```cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

void make_request_basic() {

  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] Connecting...\n");
  if (!http.begin(client, "http://swharden.com/iot/test/")) {
    Serial.println("[HTTP] Unable to connect");
  }

  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();

  Serial.printf("[HTTP] Response code %d %s\n", httpCode, http.errorToString(httpCode).c_str());

  String payload = http.getString();
  Serial.println(payload);

  http.end();
}
```

## HTTPS Request
```cpp
const char* host = "swharden.com";
const char* endpoint = "/iot/test/write/";

void make_request() {

  WiFiClientSecure client;
  client.setInsecure(); // WARNING: HTTP security disabled

  if (!client.connect(host, 443)) {
    Serial.println("ERROR: connection failed");
    return;
  }

  String url = String(endpoint) + "?data=123";
  String request = "GET " + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  client.print(request);
}
```
