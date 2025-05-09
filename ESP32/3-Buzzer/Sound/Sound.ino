/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */

#include "pitches.h"
const int BUZZZER_PIN = 18; // GIOP18 pin connected to piezo buzzer

// notes in the melody:
int melody[] = {
  NOTE_F1,NOTE_F2
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
  4,4
};

void setup() {
  // // iterate over the notes of the melody:
  // int size = sizeof(noteDurations) / sizeof(int);

  // for (int thisNote = 0; thisNote < size; thisNote++) {

  //   // to calculate the note duration, take one second divided by the note type.
  //   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  //   int noteDuration = 1000 / noteDurations[thisNote];
  //   tone(BUZZZER_PIN, melody[thisNote], noteDuration);

  //   // to distinguish the notes, set a minimum time between them.
  //   // the note's duration + 30% seems to work well:
  //   int pauseBetweenNotes = noteDuration * 1.30;
  //   delay(pauseBetweenNotes);
  //   // stop the tone playing:
  //   noTone(BUZZZER_PIN);
  // }
}

void loop() {
  // iterate over the notes of the melody:
  int size = sizeof(noteDurations) / sizeof(int);

  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZZER_PIN);
  }
  delay(500);

}