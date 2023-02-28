#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define PIN_LED_R 0
#define PIN_LED_G 1
#define PIN_LED_B 2 
#define PIN_BTN_R 3
#define PIN_BTN_G 4
#define PIN_BTN_B 5

const char* ssid = "ieeehotspot";
const char* password = "Jk638td9og35";

const char* target_server_ip = "192.168.1.184";
ESP8266WebServer server(80);

IPAddress ip(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

int button_r = 0;
int button_g = 0;
int button_b = 0;

void setup() {
    Serial.begin(115200);

    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Setup led pin http control
    pinMode(PIN_LED_R,OUTPUT);
    pinMode(PIN_LED_G,OUTPUT);
    pinMode(PIN_LED_B,OUTPUT);
    server.on("/R/on",  [](){digitalWrite(PIN_LED_R,1);});
    server.on("/R/off", [](){digitalWrite(PIN_LED_R,0);});
    server.on("/G/on",  [](){digitalWrite(PIN_LED_G,1);});
    server.on("/G/off", [](){digitalWrite(PIN_LED_G,0);});
    server.on("/B/on",  [](){digitalWrite(PIN_LED_B,1);});
    server.on("/B/off", [](){digitalWrite(PIN_LED_B,0);});

    pinMode(PIN_BTN_R,INPUT_PULLUP);
    pinMode(PIN_BTN_G,INPUT_PULLUP);
    pinMode(PIN_BTN_B,INPUT_PULLUP);

    // #TODO add interrupts for the buttons, not necessary but is more elegant
    // attachInterrupt(digitalPinToInterrupt(PIN_BTN_R),httpGETRequest(target_server_ip,"/R/on"),FALLING);
}

String httpGETRequest(const char* server,const char*  path) {
    WiFiClient client;
    HTTPClient http;

    String server_name = "https://";
    server_name.concat(server);
    server_name.concat(path);

    http.begin(client, server_name.c_str());
    int http_response_code = http.GET();
    
    String payload = "--"; 
    
    if (http_response_code>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(http_response_code);
        payload = http.getString();
    }
    else {
        Serial.print("Error code: ");
        Serial.println(http_response_code);
    }
    http.end();

    return payload;
}

void loop() {
    server.handleClient();

    if(button_r != digitalRead(PIN_BTN_R)) {
        button_r = digitalRead(PIN_BTN_R);
        if(button_r) {
            httpGETRequest(target_server_ip,"/R/on");
        } else {
            httpGETRequest(target_server_ip,"/R/off");
        }
    };

    if(button_g != digitalRead(PIN_BTN_G)) {
        button_g = digitalRead(PIN_BTN_G);
        if(button_g) {
            httpGETRequest(target_server_ip,"/G/on");
        } else {
            httpGETRequest(target_server_ip,"/G/off");
        }
    };

    if(button_b != digitalRead(PIN_BTN_B)) {
        button_b = digitalRead(PIN_BTN_B);
        if(button_b) {
            httpGETRequest(target_server_ip,"/B/on");
        } else {
            httpGETRequest(target_server_ip,"/B/off");
        }
    };
}