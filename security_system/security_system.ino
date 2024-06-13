#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include "pitches.h"

#define RFID_SS_PIN 10
#define RFID_RST_PIN A1
#define SERVO A3
#define KLINGEL_PIN A2
#define BUZZER A1
#define TASER_PIN A0
#define GREEN_LIGHT 1
#define RED_LIGHT 0

// Keypad
char C1, C2, C3, C4;
const byte COLS = 4;
const byte ROWS = 4;
int z1=0, z2, z3, z4;
char hexaKeys[ROWS][COLS] = {
  {'D','#','0','*'},
  {'C','9','8','7'},
  {'B','6','5','4'},
  {'A','3','2','1'}
};
byte colPins[COLS] = {2,3,4,5};
byte rowPins[ROWS] = {6,7,8,9};
char pressedKey;

Keypad keyfield = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
Servo servo;
MFRC522 rfidScanner(RFID_SS_PIN, RFID_RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Users
struct AuthorizedUser {
  long chipId;
  String username;
  char code[4];
};

AuthorizedUser authorizedUsers[3] = {
  {2562190, "Marlon", {'1', '2', '3', 'A'}},
  {389620, "Ferdi", {'A', 'B', 'B', 'A'}},
  {2076320, "Jakob", {'3', '1', '4', '8'}}
};

int remainingAttempts = 3;
bool inAuthenticationProcess = false;
String verificationMessage = "Code: ";

void setup() {
  Serial.begin(9600);
  Serial.println("SuperSecureSystem SSS v0.1 by Ferdinand, Marlon, Jakob");
  SPI.begin();
  rfidScanner.PCD_Init();
  servo.attach(SERVO);
  lcd.init();
  lcd.backlight();

  pinMode(RED_LIGHT, OUTPUT);
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TASER_PIN, OUTPUT);
  pinMode(KLINGEL_PIN, INPUT);
}

AuthorizedUser* getUserByCardId(long id) {
  for (int i = 0; i < (sizeof(authorizedUsers)/sizeof(authorizedUsers[0])); i++) {
    if (authorizedUsers[i].chipId == id) {
      return &authorizedUsers[i];
    }
  }
  return NULL;
}

void printCenteredText(String text, String text2) {
  lcd.clear();
  if (text != "") {
    int cursorPos = (16 - text.length()) / 2; 
    lcd.setCursor(cursorPos, 0);
    lcd.print(text);
  }
  if (text2 != "") {
    int cursorPos = (16 - text2.length()) / 2; 
    lcd.setCursor(cursorPos, 1);
    lcd.print(text2);
  }
}

void printCenteredText(String text) {
  printCenteredText(text, "");
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


void accessDeniedSound() {
  tone(BUZZER, NOTE_E7);
  delay(50);
  noTone(BUZZER);
  delay(50);
  tone(BUZZER, NOTE_E7);
  delay(100);
  noTone(BUZZER);
  tone(BUZZER, NOTE_E7);
  delay(50);
  noTone(BUZZER);
}

void execution() {
  printCenteredText("INITIATING SELF", "DEFENCE PROTOCOL!");
  delay(3000);
  printCenteredText("3");
  delay(1000);
  printCenteredText("2");
  delay(1000);
  printCenteredText("1");
  delay(1000);
  printCenteredText("DIE!!!");
  digitalWrite(TASER_PIN, HIGH);
  delay (1500);
  digitalWrite(TASER_PIN, LOW);
  delay (1500);
  lcd.clear();
}

AuthorizedUser* currentUser;

void verifyCode(AuthorizedUser* user) {
  inAuthenticationProcess = true;
  Serial.println("Starting code verification");
  lcd.clear();
  lcd.print(verificationMessage);
  currentUser = user;
  verificationLoop();
}


void playAccessGrantedJingle() {
  tone(BUZZER, NOTE_E6);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_G6);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_A6);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_B6);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_E7);
  delay(200);
  noTone(BUZZER);
}

void playAccessDeniedJingle() {
  tone(BUZZER, NOTE_E7);
  delay(100);
  noTone(BUZZER);
  delay(100);

  tone(BUZZER, NOTE_D7);
  delay(100);
  noTone(BUZZER);
  delay(100);

  tone(BUZZER, NOTE_C7);
  delay(300);
  noTone(BUZZER);
}

void verificationLoop() {
  verificationStart:
  pressedKey = keyfield.getKey();
  if (!pressedKey) { return; }
  tone(BUZZER, NOTE_E7);
  delay(50);
  noTone(BUZZER);
  if (pressedKey == '*') {
    inAuthenticationProcess = false;
    Serial.println("Aborted Code verification.");
    printCenteredText("Aborted", "Verification");
    delay(1000);
    lcd.clear();
    return;
  }

  if (pressedKey == '#') {
    printCenteredText("Input", "Resetted");
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

    if (C1 == currentUser->code[0] && C2 == currentUser->code[1] && C3 == currentUser->code[2] && C4 == currentUser->code[3]) {
      Serial.println("Code correct!");
      //servoblau.write(0);
      digitalWrite(RED_LIGHT, LOW);
      digitalWrite(GREEN_LIGHT, HIGH);
      printCenteredText("Access granted");
      delay(300);
      String message = "Welcome " + currentUser->username;
      printCenteredText("Access granted", message);
      playAccessGrantedJingle();
      delay(2000);
      lcd.clear();
      currentUser = NULL;
      inAuthenticationProcess = false;
      z1=0; z2=1; z3=1; z4=1;
      verificationMessage = "Code: ";
    } else {
      Serial.println ("Code falsch");
      digitalWrite(RED_LIGHT, HIGH);
      digitalWrite(GREEN_LIGHT, LOW);
      printCenteredText("Access denied!");
      playAccessDeniedJingle();
      delay(2000);
      z1=0; z2=1; z3=1; z4=1;
      verificationMessage = "Code: ";
      lcd.clear();
      lcd.print(verificationMessage);
      goto verificationStart;
    }
  }
}


void loop() {

  if (inAuthenticationProcess == true) {
    verificationLoop();
    return;
  }

  lcd.setCursor(5, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Insert Chipcard");

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
  if (!rfidScanner.PICC_IsNewCardPresent()) { return; }
  if (!rfidScanner.PICC_ReadCardSerial()) { return; }
 
  long currentId = 0;
  for (byte i = 0; i < rfidScanner.uid.size; i++) {
    currentId=((currentId + rfidScanner.uid.uidByte[i]) * 10);
  }
  Serial.print("Chipcard scanned! ID: ");
  Serial.println(currentId);
  AuthorizedUser* user = getUserByCardId(currentId);
  // Access denied
  if (user == NULL) {
    remainingAttempts -= 1;
    Serial.println("Zu dieser ID wurde kein Systemeintrag gefunden.");
    Serial.print("Verbleibende Versuche: ");
    Serial.println(remainingAttempts);
    digitalWrite(RED_LIGHT, HIGH);
    printCenteredText("Access denied!");
    accessDeniedSound();
    delay(2000);
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
  delay(100);
  printCenteredText("Chipcard", "Verified");
  accessGrantedSound();
  delay(2000);
  digitalWrite (GREEN_LIGHT, LOW);
  lcd.clear();
  verifyCode(user);
}