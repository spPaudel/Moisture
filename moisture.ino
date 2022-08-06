#include <Ethernet.h>
#include <SPI.h>
#include <aJSON.h>
#include <Simpletimer.h>


//  configuration of pins
int mInPin = 0;
int mOutPinTop = 2;
int mOutPinBottom = 3;
int vOutPin = 9;

//  time to poll for water readings and sending to the server
unsigned long pollMills = 60000; // 1 min

/*
  How dry the soil should get before watering.
  (Max) 1023 = Drier than the Atacama Desert
        400  = Damp
  (Min) 0    = Have you used copper instead of soil? 
*/
int waterAtMoisture = 1023 - 400;

// duration of valve opening
int waterForMills = 3500;

// ip addr of server
byte serverIP[] = { 54, 247, 99, 12 };

//  your network config
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 1, 1 }; 
byte submask[] = { 255, 255, 255, 0 };
byte gateway[] = { 192, 168, 1, 254 };
byte dns[] = { 192, 168, 1, 254 };

EthernetClient client;
Simpletimer timer;

void setup() {
  pinMode(mOutPinTop,OUTPUT);
  pinMode(mOutPinBottom,OUTPUT);
  pinMode(vOutPin, OUTPUT);
  
  Serial.begin(9600);
  
  Ethernet.begin(mac, ip, dns, gateway, submask);
  
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  //timer._cb = poll;
  //timer.run(pollMillis);
  //timer.setInterval(pollMillis, poll);
  timer.register_callback(poll);
}

void loop() {  
  timer.run(pollMills);
}

/*
  function which is run once every polling
*/
void poll() {
  Serial.print("Polling... ");
  
  int moisture = getSoilMoisture();
  int watered = 0;
  
  Serial.println(moisture);
  
  if(moisture > waterAtMoisture) {
    waterThePlant();
    watered = 1;
  }
  
  postToServer(moisture, watered);
}

/*
  Drives a current though two digital pins
  and reads the resistance through the
  analogue pin
*/
int getSoilMoisture(){

  //current flow through the divider in only one direction
  digitalWrite(mOutPinTop,HIGH);
  digitalWrite(mOutPinBottom,LOW);
  delay(1000);

  //reading from sensors
  int reading = analogRead(mInPin);

  //current is reversed
  digitalWrite(mOutPinTop,LOW);
  digitalWrite(mOutPinBottom,HIGH);
  delay(1000);

  //current is stopped
  digitalWrite(mOutPinBottom,LOW);

  return reading;
}

//  actual watering to the plant
void waterThePlant() {
  digitalWrite(valveOutputPin, HIGH);
  delay(waterForMills);
  digitalWrite(vOutPin, LOW);
}

/*
  send JSON document to the server 
  
  The JSON document describes the soil
  moisture and whether the plant was
  watered.
  
  [{
    "data": {
      "moisture": 600,
      "watered": 0
    }
  }]
  
*/
void postToServer(int moisture, int watered) {
  while(!client.connected()) {
    client.stop();
    Serial.println("Connecting...");
    if (client.connect(serverIP, 80)) {
      Serial.println("Connected");
    } else {
      Serial.println("Connection failed");
      delay(5000);
    }
  }
  
  aJsonObject* rootJson = aJson.createArray();
  
  aJsonObject* event = aJson.createObject();
  aJson.addItemToArray(rootJson, event);
  aJson.addStringToObject(event, "type", "moisture");
  aJsonObject* data = aJson.createObject();
  aJson.addItemToObject(event, "data", data);
  aJson.addNumberToObject(data, "moisture", moisture);
  aJson.addNumberToObject(data, "watered", watered);
  
  char* moistureJsonStr = aJson.print(rootJson);
  
  client.println("POST /1.0/event/put HTTP/1.0");
  client.println("Content-Length: " + String(strlen(moistureJsonStr)));
  client.println("Connection: keep-alive");
//client.println("cube-password: test"); // for using with cube server, not recommended
  client.println();
  client.println(moistureJsonStr);
}
