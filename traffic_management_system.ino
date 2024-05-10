#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RFID.h>

#define SDA_DIO 9
#define RESET_DIO 8

RFID RC522(SDA_DIO, RESET_DIO);
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// Traffic signal pins
int redLED1 = 4;
int yellowLED1 = 3;
int greenLED1 = 2;
int redLED2 = 7;
int yellowLED2 = 6;
int greenLED2 = 5;
int redLED3 = 10;
int yellowLED3 = 11;
int greenLED3 = 12;

// Authorization data
int ambulanceIDs[] = {99, 179}; 

// Timing variables
const unsigned long normalCycleTime = 5000; // 5 seconds per signal state
unsigned long lastSignalChange = 0;
enum SignalState { RED, RED_YELLOW, GREEN, YELLOW };
SignalState currentState = RED;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  RC522.init();

  lcd.init();
  lcd.backlight();  
  lcd.print("Smart Ambulance"); 

  pinMode(redLED1, OUTPUT);
  pinMode(yellowLED1, OUTPUT);
  pinMode(greenLED1, OUTPUT);
  pinMode(redLED2, OUTPUT);
  pinMode(yellowLED2, OUTPUT);
  pinMode(greenLED2, OUTPUT);

  updateTrafficSignal(currentState); 
}

void loop() {
  if (RC522.isCard()) {
    RC522.readCardSerial();
    Serial.println("Card detected:");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card detected");

    int ID = RC522.serNum[0];

    if (isAuthorized(ID)) {
      Serial.println("Ambulance Pass");
      Emergency(); 
    } else {
      Serial.println("Unauthorized Entry");
      lcd.clear();
      lcd.print("Unauthorized");
    }
  }

  // Traffic signal simulation
  unsigned long currentTime = millis();
  if (currentTime - lastSignalChange >= normalCycleTime) {
    lastSignalChange = currentTime;
    currentState = (SignalState)((currentState + 1) % 4); // Cycle through states
    updateTrafficSignal(currentState);
  }
}

void updateTrafficSignal(SignalState state) {
  switch (state) {
    case RED:
      digitalWrite(redLED1, LOW);
      digitalWrite(yellowLED1, LOW);
      digitalWrite(greenLED1,HIGH);
      digitalWrite(redLED2, LOW);
      digitalWrite(yellowLED2, LOW);
      digitalWrite(greenLED2, HIGH);
      digitalWrite(redLED3, HIGH);
      digitalWrite(yellowLED3, LOW);
      digitalWrite(greenLED3, LOW);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("LANE 1 : GREEN");
      break;
    case RED_YELLOW:
      digitalWrite(redLED1, HIGH);
      digitalWrite(yellowLED1, HIGH);
      digitalWrite(greenLED1, LOW);
      digitalWrite(redLED2, LOW);
      digitalWrite(yellowLED2, HIGH);
      digitalWrite(greenLED2, LOW);
      digitalWrite(redLED3, HIGH);
      digitalWrite(yellowLED3, LOW);
      digitalWrite(greenLED3, LOW);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(" LANE 1 : YELLOW");
      break;
    case GREEN:
      digitalWrite(redLED1, HIGH);
      digitalWrite(yellowLED1, LOW);
      digitalWrite(greenLED1, LOW);
      digitalWrite(redLED2, LOW);
      digitalWrite(yellowLED2, LOW);
      digitalWrite(greenLED2, HIGH);
      digitalWrite(redLED3, HIGH);
      digitalWrite(yellowLED3, LOW);
      digitalWrite(greenLED3, LOW);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("LANE 2 : GREEN");
      break;
    case YELLOW:
      digitalWrite(redLED1, HIGH);
      digitalWrite(yellowLED1, LOW);
      digitalWrite(greenLED1, LOW);
      digitalWrite(redLED2, HIGH);
      digitalWrite(yellowLED2, LOW);
      digitalWrite(greenLED2, LOW);
      digitalWrite(redLED3, LOW);
      digitalWrite(yellowLED3, LOW);
      digitalWrite(greenLED3, HIGH);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("LANE 3: GREEN");
      break;
  }
}

void Emergency() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EMERGENCY");
  currentState = GREEN; // Immediately switch to green
  updateTrafficSignal(currentState);
  delay(5000); // Hold green for 3 seconds
}

bool isAuthorized(int id) {
  for (int i = 0; i < sizeof(ambulanceIDs) / sizeof(ambulanceIDs[0]); i++) {
    if (ambulanceIDs[i] == id) {
      return true;
    }
  }
  return false;
}