#include "pitches.h"

int ok_melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int ok_durations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int ok_nr_notes = 8;

int fail_melody[] = {
  NOTE_AS4, NOTE_A4, NOTE_GS4, NOTE_G4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int fail_durations[] = {
   2, 2, 2, 1
};

int fail_nr_notes = 4;


void playMelody(int* melody, int* durations, int nrNotes) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < nrNotes; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / durations[thisNote];
    tone(pinSpeaker, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(pinSpeaker);
  }
}
