#include <Wire.h>
#include <sensor.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <ESP8266WiFi.h> 
#define BNO055_SAMPLERATE_DELAY_MS (100)
#define BNO055_ADDRESS_A (0x28)
#define BNO055_ADDRESS_B (0x29)
Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS_A);
Adafruit_BNO055 bno2 = Adafruit_BNO055(56, BNO055_ADDRESS_B);
sensor s[2]={sensor(),sensor()};

#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

int i=0;

const char* ssid = "hihi"; //Enter SSID
const char* password = "hihihihi"; //Enter Password
const char* websockets_server_host = "luffy.ee.ncku.edu.tw"; //Enter server adress
const uint16_t websockets_server_port = 19001; // Enter server port

using namespace websockets;

WebsocketsClient client;
void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(A0));
    // Connect to wifi
    WiFi.begin(ssid, password);

    bno.enableAutoRange(true);
    bno2.enableAutoRange(true);
    bno.begin();
    bno2.begin();
    
    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // Check if connected to wifi
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");
        return;
    }

    Serial.println("Connected to Wifi");
    // try to connect to Websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    if(connected) {
        Serial.println("Connecetd!");
        client.send("Hello");
    } else {
        Serial.println("Not Connected!");
    }
    
    // run callback when messages are received
    client.onMessage([&](WebsocketsMessage message) {
        Serial.print("Got Message: ");
        Serial.println(message.data());
    });
}

void loop() {
    
    //Serial.println(ecg[i]+random(-25,25));    
    
    sensors_event_t event;
    bno.getEvent(&event);
    sensors_event_t event2;
    bno2.getEvent(&event2);

    s[0].x=event.orientation.x;
    s[0].y=event.orientation.y;
    s[0].z=event.orientation.z;

    s[1].x=event2.orientation.x;
    s[1].y=event2.orientation.y;
    s[1].z=event2.orientation.z;

    s[0].calibrate();
    s[1].calibrate();
    
    Serial.println(",");
    Serial.print(s[0].x, 4);
    Serial.print(",");
    Serial.print(s[0].y, 4);
    Serial.print(",");
    Serial.print(s[0].z, 4);
    
    Serial.print(",");
    Serial.print(s[1].x, 4);
    Serial.print(",");
    Serial.print(s[1].y, 4);
    Serial.print(",");
    Serial.print(s[1].z, 4);
    
    Serial.println();
    
   
    
    String trans=String(ecg[i]+random(-25,25))+","+String(s[0].x,1)+","+String(s[0].y,1)+","+String(s[0].z,1)+","
           +String(s[1].x,1)+","+String(s[1].y,1)+","+String(s[1].z,1);
    
    Serial.println(trans);
    
    // let the websockets client check for incoming messages
    
    if(client.available()) {
        client.send(trans);
        Serial.println("yes");
    }
    if(i<202){
      i++;
    }
    else{
      i=0;
    }
    delay(100);
}
