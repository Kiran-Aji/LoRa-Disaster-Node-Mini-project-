#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- EDIT THESE FOR EACH NODE ---
const String MY_ID = "N01";      // Set this to "N02" for your second device
const String TARGET_ID = "N02";  // Set this to "N01" for your second device

// --- HARDWARE CONFIG ---
#define ss 15      
#define rst 16     
#define dio0 -1    
#define BUTTON_PIN A0 

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- MODES & DATA ---
enum Mode { BOOT, HOME, PREBUILD, CUSTOM };
Mode currentMode = BOOT;

String quickMsgs[] = {"SOS: HELP!", "NEED WATER", "NEED FOOD", "MEDICAL", "I AM SAFE"};
int menuIdx = 0;
const char alphabet[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789<"; 
int alphaIdx = 0;
String draft = "";
bool loraOnline = false;

// --- TIMING ---
unsigned long bootTimer;
unsigned long lastAction = 0;
unsigned long lastMidPress = 0;
const int debounce = 350;     

// --- PROTOTYPES ---
void render();
void resetVars();
void transmit(String m);
void checkIncoming();
void showBoot();
void handleInput(int v);

void setup() {
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);
  loraOnline = LoRa.begin(433E6);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showBoot(); 
  bootTimer = millis(); 
}

void loop() {
  checkIncoming();

  if (currentMode == BOOT) {
    if (millis() - bootTimer > 3000) { currentMode = HOME; render(); }
    return;
  }

  int val = analogRead(BUTTON_PIN);
  if (val < 100) return; 

  if (millis() - lastAction > debounce) {
    handleInput(val);
    lastAction = millis();
    render();
  }
}

void handleInput(int v) {
  // CALIBRATED VALUES
  bool bUp    = (v > 400 && v < 480);   // 440
  bool bMid   = (v > 500 && v < 600);   // 543
  bool bLeft  = (v > 700 && v < 800);   // 737
  bool bDown  = (v > 940 && v < 1000);  // 989
  bool bRight = (v > 1010);             // 1024

  bool singleClick = false;
  bool doubleClick = false;

  if (bMid) {
    unsigned long now = millis();
    if (now - lastMidPress < 700 && now - lastMidPress > 100) {
      doubleClick = true; lastMidPress = 0;
    } else {
      lastMidPress = now; singleClick = true; 
    }
  }

  if (currentMode == HOME) {
    if (bUp) menuIdx = 0;   
    if (bDown) menuIdx = 1; 
    if (singleClick) {
      currentMode = (menuIdx == 0) ? PREBUILD : CUSTOM;
      resetVars();
    }
  } 
  else if (currentMode == PREBUILD) {
    if (bUp) menuIdx = (menuIdx - 1 + 5) % 5;
    if (bDown) menuIdx = (menuIdx + 1) % 5;
    if (doubleClick) { transmit(quickMsgs[menuIdx]); currentMode = HOME; }
    if (bLeft) { currentMode = HOME; render(); }
  } 
  else if (currentMode == CUSTOM) {
    if (bLeft) alphaIdx = (alphaIdx - 1 + 38) % 38;
    if (bRight) alphaIdx = (alphaIdx + 1) % 38;
    if (doubleClick && draft.length() > 0) { 
      transmit(draft); currentMode = HOME; 
    }
    else if (singleClick) {
      char c = alphabet[alphaIdx];
      if (c == '<') { if (draft.length() > 0) draft.remove(draft.length()-1); }
      else if (draft.length() < 16) draft += c;
    }
    if (bUp) { currentMode = HOME; render(); }
  }
}

void render() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  
  if (currentMode == HOME) {
    display.setCursor(35, 0); display.println("MAIN MENU");
    display.drawFastHLine(0, 10, 128, WHITE);
    display.setCursor(0, 25); display.print(menuIdx == 0 ? "> " : "  "); display.println("PREBUILD TEXT");
    display.setCursor(0, 45); display.print(menuIdx == 1 ? "> " : "  "); display.println("CUSTOM TYPING");
  } 
  else if (currentMode == PREBUILD) {
    display.setCursor(0, 0); display.println("SELECT:");
    display.setTextSize(2); display.setCursor(0, 25); display.println(quickMsgs[menuIdx]);
    display.setTextSize(1); display.setCursor(0, 55); display.println("DBL-MID TO SEND");
  } 
  else if (currentMode == CUSTOM) {
    display.setCursor(0, 0); display.print("MSG: "); display.println(draft);
    display.setTextSize(2); 
    display.setCursor(55, 36); // Raised
    display.print(alphabet[alphaIdx]);
    display.drawFastHLine(53, 53, 16, WHITE); // Closer line
    display.setTextSize(1); display.setCursor(0, 58); display.print("SNG:Add | DBL:Send");
  }
  display.display();
}

void transmit(String m) {
  display.clearDisplay();
  display.setCursor(15, 25); display.println("SENDING TO " + TARGET_ID);
  display.display();

  LoRa.beginPacket();
  // Format: SENDER:TARGET:MESSAGE
  LoRa.print(MY_ID + ":" + TARGET_ID + ":" + m); 
  LoRa.endPacket();

  delay(1000);
  display.clearDisplay();
  display.setCursor(15, 25); display.println("SENT & ACKNOWLEDGED");
  display.display();
  delay(1500);
  resetVars();
}

void checkIncoming() {
  if (LoRa.parsePacket()) {
    String raw = "";
    while (LoRa.available()) raw += (char)LoRa.read();
    
    // Parse Format: SENDER:TARGET:MESSAGE
    int firstColon = raw.indexOf(':');
    int secondColon = raw.indexOf(':', firstColon + 1);
    
    if (firstColon != -1 && secondColon != -1) {
      String sender = raw.substring(0, firstColon);
      String target = raw.substring(firstColon + 1, secondColon);
      String content = raw.substring(secondColon + 1);

      // --- SECURITY CHECK: Only show if meant for ME and from my TARGET ---
      if (target == MY_ID && sender == TARGET_ID) {
        display.clearDisplay();
        display.setTextSize(1); display.setCursor(0,0); display.println("INCOMING:");
        display.setTextSize(2); display.setCursor(0,20); display.println(content);
        display.setTextSize(1); 
        display.setCursor(0, 48); display.print("FROM: "); display.print(sender);
        display.setCursor(0, 57); display.println(">> MSG RECEIVED ACK <<"); 
        display.display();
        delay(5000);
        render();
      }
    }
  }
}

void resetVars() { draft = ""; alphaIdx = 0; menuIdx = 0; }

void showBoot() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(20, 10); display.println("DISASTER NODE ON");
  display.drawFastHLine(0, 22, 128, WHITE);
  display.setCursor(10, 35); display.print("ID: "); display.println(MY_ID);
  display.setCursor(10, 50); display.print("TARGET: "); display.println(TARGET_ID);
  display.display();
}
