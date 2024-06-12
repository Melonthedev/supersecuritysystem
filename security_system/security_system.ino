#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include "pitches.h"

#define SS_PIN 10
#define RST_PIN A1
#define KLINGEL_PIN A2
#define BUZZER 3
#define TASER_PIN 11
#define GREEN_LIGHT 2
#define RED_LIGHT 4


char P1='1';char P2='2';char P3='3';char P4='A';
char C1, C2, C3, C4;
const byte COLS = 4;
const byte ROWS = 4;
int z1=0, z2, z3, z4;
char hexaKeys[ROWS][COLS]={
{'D','#','0','*'},
{'C','9','8','7'},
{'B','6','5','4'},
{'A','3','2','1'}
};

byte colPins[COLS] = {2,3,4,5};
byte rowPins[ROWS] = {6,7,8,9};
char pressedKey; //Taste ist die Variable für die jeweils gedrückte Taste.
Keypad keyfield = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
Servo servo;
MFRC522 rfidScanner(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);


struct AuthorizedUsers {
  long chipId;
  String username;
};

AuthorizedUsers authorizedUsers[3] = {
  {2048270, "Marlon"},
  {480830, "Ferdi"},
  {2562190, "Blau"}
};

int remainingAttempts = 3;
bool inAuthenticationProcess = false;
String verificationMessage = "Code: ";

void setup(){
  Serial.begin(9600);
  Serial.println("SuperSecureSystem SSS v0.1 by Ferdinand, Marlon, Jakob");
  SPI.begin();
  rfidScanner.PCD_Init();
  servo.attach(A3);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(TASER_PIN, OUTPUT);
  pinMode(KLINGEL_PIN, INPUT);
  
  lcd.init();
  lcd.backlight();
}

String getNameByCardId(long id) {
  for (int i = 0; i < (sizeof(authorizedUsers)/sizeof(authorizedUsers[0])); i++) {
    if (authorizedUsers[i].chipId == id) {
      return authorizedUsers[i].username;
    }
  }
  return ""; // Leerer String, wenn die ID nicht gefunden wird
}


void accessGrantedSound() {
  tone(BUZZER, NOTE_E7);
  delay(50);
  noTone(BUZZER);
  delay(50);
  tone(BUZZER, NOTE_G7);
  delay(100);
  noTone(BUZZER);
  tone(BUZZER, NOTE_C8);
  delay(50);
  noTone(BUZZER);
}


void execution() {
  lcd.clear();
  lcd.print ("INITIATING SELF");
  lcd.setCursor(0, 1);
  lcd.print("DEFENCE PROTOCOL!");
  delay(3000);
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("3");
  delay(1000);
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("2");
  delay(1000);
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("1");
  delay(1000);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("DIE!!!");
  /*lcd.setCursor(7, 1);
  lcd.print(">:(");*/
  delay (3000);
  lcd.clear();
}

String currentUsername;

void verifyCode(String username) {
  Serial.println("Starting code verification");
  inAuthenticationProcess = true;
  lcd.clear();
  lcd.print(verificationMessage);
  currentUsername = username;
  verificationLoop();
}


void verificationLoop() {
  verificationStart:
  //lcd.clear();
  //lcd.print(verificationMessage);
  pressedKey = keyfield.getKey();
  if (!pressedKey) { return; }
  if (pressedKey == '*') {
   Serial.println("Aborted Code verification.");
   lcd.clear();
   lcd.setCursor(4, 0);
   lcd.print("Aborted");
   lcd.setCursor(2, 1);
   lcd.print("Verification");
   inAuthenticationProcess = false;
   return;
  }
  if (pressedKey == '#') {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Input");
    lcd.setCursor(4, 1);
    lcd.print("Resetted");
    delay(2000);
    lcd.clear();
    z1=0; z2=1; z3=1; z4=1;
    verificationMessage = "Code: ";
    lcd.clear();
    lcd.print(verificationMessage);
    goto verificationStart;
  }
  verificationMessage = verificationMessage + "*";
  lcd.clear();
  lcd.print(verificationMessage);
  if (z1 == 0) {
    C1 = pressedKey;
    z1=1; z2=0; z3=1; z4=1;
    goto verificationStart;
  }
  if (z2 == 0) {
    C2 = pressedKey;
    z1=1; z2=1; z3=0; z4=1;
    goto verificationStart;
  }
  if (z3 == 0) {
    C3 = pressedKey;
    z1=1; z2=1; z3=1; z4=0;
    goto verificationStart;
  }
  if (z4 == 0) {
    C4 = pressedKey;
    z1=1; z2=1; z3=1; z4=1;
    if (C1 == P1 && C2 == P2 && C3 == P3 && C4 == P4) {
     Serial.println("Code korrekt, Schloss offen");
     //servoblau.write(0);
     digitalWrite(RED_LIGHT, LOW);
     digitalWrite(GREEN_LIGHT, HIGH);
     lcd.clear();
     lcd.setCursor(1, 0);
     lcd.print("Access granted");
     delay(300);
     String message = "Welcome " + currentUsername;
     int cursorPos = (16 - message.length()) / 2; 
     lcd.setCursor(cursorPos, 1);
     lcd.print(message);
     delay(2000);
     lcd.clear();
     currentUsername = "";
     inAuthenticationProcess = false;
    } else {
     Serial.println ("Code falsch, Schloss gesperrt");
     digitalWrite(RED_LIGHT, HIGH);
     digitalWrite(GREEN_LIGHT, LOW);
     lcd.clear();
     lcd.setCursor(1, 0);
     lcd.print("Access denied!");
     delay(2000);
     lcd.clear();
     z1=0; z2=1; z3=1; z4=1;
     verificationMessage = "Code: ";
     lcd.clear();
     lcd.print(verificationMessage);
     goto verificationStart;
    }
  }
}


void loop(){

  if (inAuthenticationProcess == true) {
    verificationLoop();
    return;
  }
  
  lcd.setCursor(5, 0);
  lcd.print("Welcome");
  lcd.setCursor(1, 1);
  lcd.print("Insert keycard");

  // Klingel
  if (digitalRead(KLINGEL_PIN) == HIGH) {
    lcd.clear();
    lcd.print("Ringing...");
    tone(BUZZER, NOTE_E5);
    delay(400);
    tone(BUZZER, NOTE_C5);
    delay(600);
    noTone(BUZZER);
    delay(1000);
    lcd.clear();
  }
 
  // NFC Kontrolle  
  if (!rfidScanner.PICC_IsNewCardPresent()){ return; }
  if (!rfidScanner.PICC_ReadCardSerial()){ return; }
 
  long currentId = 0;
  for (byte i = 0; i < rfidScanner.uid.size; i++) {
    currentId=((currentId + rfidScanner.uid.uidByte[i]) * 10);
  }
  Serial.print("Kartennummer: ");
  Serial.println(currentId);
  String username = getNameByCardId(currentId);
  if (username == "") {
    // Access denied
    remainingAttempts -= 1;
    Serial.println("Kein Eintrag im system zu dieser Kartennummer gefunden.");
    Serial.print("Verbleibende Versuche: ");
    Serial.println(remainingAttempts);
    digitalWrite (BUZZER, HIGH);
    digitalWrite (RED_LIGHT, HIGH);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Access denied!");
    delay (2000);
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Remaining");
    lcd.setCursor(2, 1);
    lcd.print("attempts: ");
    lcd.setCursor(12, 1);
    lcd.print(remainingAttempts);
    delay (2000);
    if (remainingAttempts <= 0) {
      remainingAttempts = 3;
      execution();
    }
    lcd.clear();
    return;
  }
  // Access granted
  digitalWrite (GREEN_LIGHT, HIGH); 
  delay (100);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Access granted");
  accessGrantedSound();
  
  /*digitalWrite (3, LOW);
  delay (100);
  digitalWrite (3, HIGH);
  delay (100);
  digitalWrite (3, LOW);*/
  
  delay (2000);
  digitalWrite (GREEN_LIGHT, LOW);
  verifyCode(username);
}