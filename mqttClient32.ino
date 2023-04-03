//Add Library
#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define CONNECTION_TIMEOUT 10

//Wifi SSID
const char *ssid = "Wumbo", *pw = "wumbo@mumbo";

char server[255];

//Servo Object
Servo Claw;

//WifiManager Object
WiFiManager wm;
WiFiClient espClient;

//MQTT Object
PubSubClient client(espClient);

//Changeable variable
int tryDelay = 500, port, degree = 0, servoPort;
char espId[15];
String mainTopic;

void moveServo(Servo hand, int target, int& degree = degree, int increment = 15){
  int minInt = 0, maxInt = 180; 
  if(target > maxInt || target < minInt || !degree){
    Serial.println("Not valid integer");
  }
  else if (degree < target){
    if(degree == maxInt)
      ;
    else{
      for(degree; degree <= target; degree += increment){
        hand.write(degree);
      }
      degree = target;
    }
  }
  else if(degree > target){
    if(degree == minInt);
    else{
      for(degree; degree >= target; degree -= increment){
        hand.write(degree);
      }
      degree = target;
    }
  }
}

//MQTT CALLBACK
/*
  This one is called if the client receive a message
*/
void callback(char* topic, byte* payload, unsigned int length) {
  char response[length + 1];
  memcpy(response, payload, length);
  response[length] = '\0';
  int responseVal = atoi(response);
  Serial.println(response);
  Serial.println(topic);
  if(strcmp(topic, mainTopic.c_str()) == 0){
    if(responseVal == 10000){
      moveServo(Claw, 90);
    }
    if(responseVal == 10500){
      moveServo(Claw, 0);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP ON!");
  Claw.attach(servoPort);
  WiFi.mode(WIFI_STA);
  bool res;
  wm.setDebugOutput(false);
  wm.resetSettings();
  WiFiManagerParameter mqtt_server("server_address", "Enter MQTT Server Address", "0.0.0.0", 255);
  wm.addParameter(&mqtt_server);
  WiFiManagerParameter mqtt_port("port_address", "Enter MQTT Port", "1883", 255);
  wm.addParameter(&mqtt_port);
  res = wm.autoConnect("ESP CONFIG", "siotics0rgConf");
  if (!res) {
    Serial.println("Failed to connect");
    ESP.restart();
    delay(1000);
  } else {
    Serial.print("Connected to Wifi with IP: ");
    Serial.println(WiFi.localIP());
  }
  Serial.print("Server IP: ");
  strcpy(server, mqtt_server.getValue());
  port = atoi(mqtt_port.getValue());
  Serial.println(server);
  delay(tryDelay);
  sprintf(espId, "%06X", ESP.getEfuseMac());
  client.setServer(server, port);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect(espId)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Failed to connect. Retrying...");
      delay(1000);
    }
  }
  mainTopic = "esp/" + String(espId) + "/response";
  client.subscribe(mainTopic.c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi disconnected");
    Serial.println("Retrying...");
    WiFi.begin(ssid, pw);
    delay(5000);
  }

  if (!client.connected()) {
    reconnect();
    return;
  }
  client.loop();
  /*
  if (hexID.length() != 0) {
    String attendance = "esp/attendance";
    String espIdStr = espId;
    String hexIDStr = hexID;
    String msg = "{\"espId\":\"" + espIdStr + "\",\"hexID\":\"" + hexIDStr + "\"}";
    client.publish(attendance.c_str(), msg.c_str());
    hexID = "";
  }
  */
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(espId)) {
      Serial.println("connected");
      String topic = "esp/" + String(espId) + "/response";
      client.subscribe(topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}