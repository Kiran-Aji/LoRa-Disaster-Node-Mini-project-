#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ss 15      
#define rst 16     
#define dio0 -1    

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


String myID;  
long lastSendTime = 0;        
int interval = 2000;          
int msgCount = 0;

void setup() {
  
  delay(3000); 

  Serial.begin(9600);
  Serial.println(">>> BROADCAST CHAT STARTING <<<");

  randomSeed(analogRead(0)); 
  myID = "Node " + String(random(10, 99)); 
  Serial.print("ID: "); Serial.println(myID);

  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6))
   {
    Serial.println("❌ LoRa Failed!");
    while (1);
   }

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
 { 
    Serial.println(F("❌ OLED Failed"));
    for(;;);
   }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("LoRa Broadcast");
  display.println("----------------");
  display.print("my ID: "); display.println(myID);
  display.display();
  delay(2000);
}

void loop() {
  onReceive(LoRa.parsePacket());

  if (Serial.available() > 0) {
    // Read the full string
    String input = Serial.readString(); 
    input.trim(); 
    
    if (input.length() > 0) {
      Serial.print("Attempting to send: "); Serial.println(input);
      sendMessage(input); 
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(">> SENT SUCCESSFULLY:");
      display.setCursor(0, 18);
      display.println(input);
      display.display();
      delay(500); // Give the user time to read the screen
    }
  }
}

void onReceive(int packetSize) {
  if (packetSize == 0) return; 

  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  
  // Debug: See the raw string in Serial
  Serial.println("Raw Data: " + incoming);

  int splitIndex = incoming.indexOf('|');
  if (splitIndex != -1) {
    String senderName = incoming.substring(0, splitIndex);
    String msgText = incoming.substring(splitIndex + 1);

    if (senderName == myID) return;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("From: "); display.println(senderName);
    display.setCursor(0, 18);
    display.println(msgText);
    display.display();
  }
}void sendMessage(String outgoing) 
{
  LoRa.beginPacket();
  LoRa.print(myID);
  LoRa.print("|");
  LoRa.print(outgoing);
  LoRa.endPacket();
}

