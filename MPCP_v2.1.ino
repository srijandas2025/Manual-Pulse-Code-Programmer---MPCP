#include <EEPROM.h>

// PIN ASSIGNMENTS
const int btnSelect = 2;  
const int btnConfirm = 3; 
const int btnErase = 5;   
const int ledBit = 4;     
const int ledMode = 6;    
const int ledErase = 7;   
const int buzzer = 8;     
const int ledPlay = 13;   

// VARIABLES
int program[8];
int currentBitValue = 0;
int bitCounter = 0;
bool isProgramReady = false;

void setup() {
  pinMode(btnSelect, INPUT);
  pinMode(btnConfirm, INPUT);
  pinMode(btnErase, INPUT);
  pinMode(ledBit, OUTPUT);
  pinMode(ledMode, OUTPUT);
  pinMode(ledErase, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPlay, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("======================================");
  Serial.println("   MANUAL PULSE-CODE PROGRAMMER v2.1  ");
  Serial.println("======================================");

  if (EEPROM.read(10) == 1) {
    for (int i = 0; i < 8; i++) program[i] = EEPROM.read(i);
    bitCounter = 8;
    isProgramReady = true;
    Serial.println("STATUS: Saved program loaded.");
  }
}

void loop() {
  // 1. ERASE LOGIC
  if (digitalRead(btnErase) == HIGH) {
    Serial.println("\nACTION: !!! MEMORY ERASED !!!");
    digitalWrite(ledErase, HIGH);
    tone(buzzer, 500, 1000); 
    for (int i = 0; i < 11; i++) EEPROM.write(i, 0);
    bitCounter = 0;
    isProgramReady = false;
    currentBitValue = 0;
    delay(1000);
    digitalWrite(ledErase, LOW);
  }

  // 2. INPUT MODE
  if (bitCounter < 8) {
    digitalWrite(ledMode, HIGH);
    
    if (digitalRead(btnSelect) == HIGH) {
      currentBitValue = !currentBitValue;
      digitalWrite(ledBit, currentBitValue);
      tone(buzzer, 1000, 50); 
      Serial.print("TOGGLE -> Current Bit: "); Serial.println(currentBitValue);
      delay(300);
    }

    if (digitalRead(btnConfirm) == HIGH) {
      program[bitCounter] = currentBitValue;
      EEPROM.write(bitCounter, currentBitValue);
      Serial.print("SAVED: Bit ["); Serial.print(bitCounter);
      Serial.print("] = "); Serial.println(currentBitValue);
      bitCounter++;
      
      tone(buzzer, 1500, 100); delay(150); tone(buzzer, 1500, 100);
      digitalWrite(ledBit, LOW); delay(100);
      digitalWrite(ledBit, currentBitValue);
      
      if (bitCounter == 8) {
        EEPROM.write(10, 1);
        isProgramReady = true;
        digitalWrite(ledMode, LOW);
        Serial.println("\n--- 8-BIT CODE COMPLETE ---");
      }
      delay(200);
    }
  } 
  // 3. PLAYBACK MODE (With Pin 13 Monitor)
  else if (isProgramReady) {
    Serial.println("\n>> EXECUTING SEQUENCE:");
    for (int i = 0; i < 8; i++) {
      if (digitalRead(btnErase) == HIGH) return;
      
      digitalWrite(ledPlay, program[i]);
      
      // THIS PRINTS THE PIN 13 STATE TO SERIAL
      Serial.print("Bit "); Serial.print(i);
      Serial.print(": Pin 13 is -> "); Serial.println(program[i]);
      
      if(program[i] == 1) tone(buzzer, 800, 100); 
      delay(800); 
      digitalWrite(ledPlay, LOW);
      delay(200);
    }
  }
}