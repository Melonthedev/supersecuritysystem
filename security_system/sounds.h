#include "pitches.h"
#define BUZZER A1

void playTone(int note, int duration) {
  tone(BUZZER, note, duration);
  delay(duration * 0.8);
  noTone(BUZZER);
}


void playAccessGrantedJingle() {
  tone(BUZZER, NOTE_E7);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_G7);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_A7);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_B7);
  delay(100);
  noTone(BUZZER);
  delay(100);
  
  tone(BUZZER, NOTE_E8);
  delay(200);
  noTone(BUZZER);
}


void playAccessDeniedJingle1() {
  playTone(NOTE_D4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);
  playTone(NOTE_G4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);

  playTone(NOTE_B4, DURATION_DOTTED_EIGHTH);
  playTone(NOTE_B4, DURATION_DOTTED_EIGHTH);

  playTone(NOTE_A4, DURATION_DOTTED_QUARTER);

  playTone(NOTE_D4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);
  playTone(NOTE_G4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);

  playTone(NOTE_A4, DURATION_DOTTED_EIGHTH);
  playTone(NOTE_A4, DURATION_DOTTED_EIGHTH);
  playTone(NOTE_G4, DURATION_DOTTED_EIGHTH);

  playTone(NOTE_FS4, DURATION_SIXTEENTH);

  playTone(NOTE_E4, DURATION_EIGHTH);
}
void playAccessDeniedJingle2() {

  playTone(NOTE_D4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);
  playTone(NOTE_G4, DURATION_SIXTEENTH);
  playTone(NOTE_E4, DURATION_SIXTEENTH);

  playTone(NOTE_G4, DURATION_QUARTER);

  playTone(NOTE_A4, DURATION_EIGHTH);

  playTone(NOTE_FS4, DURATION_DOTTED_EIGHTH);

  playTone(NOTE_E4, DURATION_SIXTEENTH);

  playTone(NOTE_D4, DURATION_EIGHTH);
  playTone(NOTE_D4, DURATION_EIGHTH);
  playTone(NOTE_D4, DURATION_EIGHTH);

  playTone(NOTE_A4, DURATION_QUARTER);
  
  playTone(NOTE_G4, DURATION_HALF);
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
