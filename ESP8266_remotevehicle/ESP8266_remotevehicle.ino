#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <Servo.h>

const char* ssid = "UPC4571729";
const char* password = "Ma6cxhcksPph";

ESP8266WebServer server(80);

Servo servoMotor1;
Servo servoMotor2;

int acceleration = 0;
int direct = 0;

void handlePostRequest() {
  String jsonString = server.arg("plain");
  Serial.println(jsonString);
 
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  
   if (error) {
    Serial.print("Parsing failed: ");
    Serial.println(error.c_str());
    server.send(500, "text/plain", "Internal Server Error");
    return;
  }
  
  if (doc.containsKey("acceleration") && doc.containsKey("direction")) {
    acceleration = doc["acceleration"].as<int>();
    direct = doc["direction"].as<int>();

    //acceleration
    int angle1 = map(acceleration, 0, 100, 0, 180);
    servoMotor1.write(angle1/10);

    //direction
    int angle2 = map(direct, -100, 100, 0, 180);
    servoMotor2.write(angle2);

    server.send(200, "text/plain", "updated");
  } else {
    server.send(400, "text/plain", "Invalid JSON payload");
  }
  server.send(200, "text/plain", "updated");

}

void setup() {
  Serial.begin(115200);

  //WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  //Print ESP IP
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin()) {
    Serial.println("Failed to initialize SPIFFS");
    return;
  }
  //Mapping for POST request
  server.on("/updatedata", HTTP_POST, handlePostRequest);
  server.begin();
  Serial.println("HTTP server started");
  
  //servo1(acceleration)
  servoMotor1.attach(D1);
  //servoMotor1.write(180); 

  //servo2(direction)
  servoMotor2.attach(D2);
  servoMotor2.write(90);  
}


void loop() {
    server.handleClient();
}
