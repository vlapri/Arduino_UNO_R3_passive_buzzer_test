// ============================================================================
// Pasivni buzzer - 12 ukazek melodii / sirenovych signalu
// Passive buzzer - 12 melody / siren demos
//
// Zapojeni / Wiring:
//   Buzzer (+)        -> D8
//   Buzzer (-)        -> GND
//   Tlacitko "Dalsi"  -> D2  (druhy pin tlacitka -> GND, pouzit INPUT_PULLUP)
//   Tlacitko "Predchozi" -> D3 (druhy pin tlacitka -> GND, pouzit INPUT_PULLUP)
//
// Podrobnosti viz README.md / See README.md for details.
// ============================================================================

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "pitches.h"

// ---------------------------------------------------------------------------
// Konfigurace jazyka konzoloveho vystupu / Console language configuration
// ---------------------------------------------------------------------------
#define LANG_CS 0
#define LANG_EN 1

// Zmenou teto konstanty prepnete jazyk vypisu do Serial monitoru.
// Change this constant to switch the Serial monitor output language.
const uint8_t LANGUAGE = LANG_CS;

// ---------------------------------------------------------------------------
// Piny / Pins
// ---------------------------------------------------------------------------
const uint8_t BUZZER_PIN = 8;
const uint8_t BUTTON_NEXT_PIN = 2; // INT0
const uint8_t BUTTON_PREV_PIN = 3; // INT1

const uint8_t MELODY_COUNT = 12;
const unsigned long DEBOUNCE_MS = 250;

// ---------------------------------------------------------------------------
// Stav ovladany prerusenimi / Interrupt-driven state
// ---------------------------------------------------------------------------
volatile bool nextRequested = false;
volatile bool prevRequested = false;
volatile unsigned long lastNextInterrupt = 0;
volatile unsigned long lastPrevInterrupt = 0;

int8_t currentMelody = -1; // -1 = zatim nic nehralo / nothing played yet

void isrNext() {
  unsigned long now = millis();
  if (now - lastNextInterrupt > DEBOUNCE_MS) {
    nextRequested = true;
    lastNextInterrupt = now;
  }
}

void isrPrev() {
  unsigned long now = millis();
  if (now - lastPrevInterrupt > DEBOUNCE_MS) {
    prevRequested = true;
    lastPrevInterrupt = now;
  }
}

// ---------------------------------------------------------------------------
// Tisk do konzole / Console output helpers
// ---------------------------------------------------------------------------
void printBoth(const __FlashStringHelper *cz, const __FlashStringHelper *en) {
  Serial.println(LANGUAGE == LANG_CS ? cz : en);
}

void printMelodyName(uint8_t idx) {
  switch (idx) {
    case 0: printBoth(F("Hasici (sirena)"), F("Firefighters (siren)")); break;
    case 1: printBoth(F("Policie (sirena)"), F("Police (siren)")); break;
    case 2: printBoth(F("Zachranka (sirena)"), F("Ambulance (siren)")); break;
    case 3: printBoth(F("Pozarni poplach"), F("Fire alarm")); break;
    case 4: printBoth(F("Ohrozeni (civilni sirena)"), F("Air-raid siren")); break;
    case 5: printBoth(F("Red Alert (Star Trek TNG)"), F("Red Alert (Star Trek TNG)")); break;
    case 6: printBoth(F("Imperial March (Star Wars)"), F("Imperial March (Star Wars)")); break;
    case 7: printBoth(F("Super Mario Bros"), F("Super Mario Bros")); break;
    case 8: printBoth(F("Tetris (Korobeiniki)"), F("Tetris (Korobeiniki)")); break;
    case 9: printBoth(F("Mise: Nemozna"), F("Mission: Impossible")); break;
    case 10: printBoth(F("Oda na radost (Beethoven)"), F("Ode to Joy (Beethoven)")); break;
    case 11: printBoth(F("SOS (Morseova abeceda)"), F("SOS (Morse code)")); break;
  }
}

void printMelodyHeader(uint8_t idx) {
  Serial.print(LANGUAGE == LANG_CS ? F("Prehravam ukazku ") : F("Playing demo "));
  Serial.print(idx + 1);
  Serial.print(F("/"));
  Serial.print(MELODY_COUNT);
  Serial.print(F(": "));
  printMelodyName(idx);
}

// ---------------------------------------------------------------------------
// Prehravac melodii z PROGMEM / PROGMEM melody player
// ---------------------------------------------------------------------------
struct Note {
  uint16_t frequency;
  uint16_t duration; // ms
};

void playMelody(const Note *melody, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    uint16_t freq = pgm_read_word(&melody[i].frequency);
    uint16_t dur = pgm_read_word(&melody[i].duration);
    if (freq > 0) {
      tone(BUZZER_PIN, freq, dur - dur / 10);
    } else {
      noTone(BUZZER_PIN);
    }
    delay(dur);
  }
  noTone(BUZZER_PIN);
}

// ---------------------------------------------------------------------------
// Notove melodie (v PROGMEM) / Note-based melodies (in PROGMEM)
// ---------------------------------------------------------------------------
const Note IMPERIAL_MARCH[] PROGMEM = {
  {NOTE_A4, 350}, {NOTE_A4, 350}, {NOTE_A4, 350}, {NOTE_F4, 250}, {NOTE_C5, 100},
  {NOTE_A4, 350}, {NOTE_F4, 250}, {NOTE_C5, 100}, {NOTE_A4, 700},
  {NOTE_E5, 350}, {NOTE_E5, 350}, {NOTE_E5, 350}, {NOTE_F5, 250}, {NOTE_C5, 100},
  {NOTE_GS4, 350}, {NOTE_F4, 250}, {NOTE_C5, 100}, {NOTE_A4, 700},
};

const Note SUPER_MARIO[] PROGMEM = {
  {NOTE_E5, 150}, {NOTE_E5, 150}, {NOTE_REST, 150}, {NOTE_E5, 150}, {NOTE_REST, 150},
  {NOTE_C5, 150}, {NOTE_E5, 150}, {NOTE_G5, 300}, {NOTE_REST, 300}, {NOTE_G4, 300},
};

const Note TETRIS[] PROGMEM = {
  {NOTE_E5, 400}, {NOTE_B4, 200}, {NOTE_C5, 200}, {NOTE_D5, 400}, {NOTE_C5, 200},
  {NOTE_B4, 200}, {NOTE_A4, 400}, {NOTE_A4, 200}, {NOTE_C5, 200}, {NOTE_E5, 400},
  {NOTE_D5, 200}, {NOTE_C5, 200}, {NOTE_B4, 600}, {NOTE_C5, 200}, {NOTE_D5, 400},
  {NOTE_E5, 400}, {NOTE_C5, 400}, {NOTE_A4, 400}, {NOTE_A4, 400},
};

const Note MISSION_IMPOSSIBLE[] PROGMEM = {
  {NOTE_G4, 150}, {NOTE_REST, 150}, {NOTE_G4, 150}, {NOTE_REST, 150},
  {NOTE_AS4, 150}, {NOTE_REST, 50}, {NOTE_C5, 150}, {NOTE_REST, 150},
  {NOTE_G4, 150}, {NOTE_REST, 150}, {NOTE_G4, 150}, {NOTE_REST, 150},
  {NOTE_AS4, 150}, {NOTE_REST, 50}, {NOTE_CS5, 150}, {NOTE_REST, 150},
  {NOTE_C5, 150}, {NOTE_REST, 50}, {NOTE_AS4, 150}, {NOTE_REST, 50}, {NOTE_G4, 300},
};

const Note ODE_TO_JOY[] PROGMEM = {
  {NOTE_E5, 400}, {NOTE_E5, 400}, {NOTE_F5, 400}, {NOTE_G5, 400},
  {NOTE_G5, 400}, {NOTE_F5, 400}, {NOTE_E5, 400}, {NOTE_D5, 400},
  {NOTE_C5, 400}, {NOTE_C5, 400}, {NOTE_D5, 400}, {NOTE_E5, 400},
  {NOTE_E5, 600}, {NOTE_D5, 200}, {NOTE_D5, 800},
};

void playImperialMarch() { playMelody(IMPERIAL_MARCH, sizeof(IMPERIAL_MARCH) / sizeof(Note)); }
void playSuperMario() { playMelody(SUPER_MARIO, sizeof(SUPER_MARIO) / sizeof(Note)); }
void playTetris() { playMelody(TETRIS, sizeof(TETRIS) / sizeof(Note)); }
void playMissionImpossible() { playMelody(MISSION_IMPOSSIBLE, sizeof(MISSION_IMPOSSIBLE) / sizeof(Note)); }
void playOdeToJoy() { playMelody(ODE_TO_JOY, sizeof(ODE_TO_JOY) / sizeof(Note)); }

// ---------------------------------------------------------------------------
// Procedurlani signaly / sireny / Procedural signals / sirens
// ---------------------------------------------------------------------------

// Hasici: rychla stoupajici/klesajici houkacka (Martin horn) / fast rising-falling wail
void playFirefighters() {
  for (uint8_t cycle = 0; cycle < 3; cycle++) {
    for (uint16_t f = 500; f <= 1200; f += 20) {
      tone(BUZZER_PIN, f);
      delay(6);
    }
    for (uint16_t f = 1200; f >= 500; f -= 20) {
      tone(BUZZER_PIN, f);
      delay(6);
    }
  }
  noTone(BUZZER_PIN);
}

// Policie: rychle strida ("yelp") / fast alternating chirps
void playPolice() {
  for (uint8_t i = 0; i < 16; i++) {
    tone(BUZZER_PIN, 650, 90);
    delay(90);
    tone(BUZZER_PIN, 1600, 90);
    delay(90);
  }
  noTone(BUZZER_PIN);
}

// Zachranka: pomale dvoutonove "hi-lo" houkani / slow two-tone hi-lo wail
void playAmbulance() {
  for (uint8_t i = 0; i < 5; i++) {
    tone(BUZZER_PIN, 950, 500);
    delay(500);
    tone(BUZZER_PIN, 750, 500);
    delay(500);
  }
  noTone(BUZZER_PIN);
}

// Pozarni poplach: 3 kratka pipnuti + pauza (typicky hlasic koure) / 3 short beeps + pause (smoke-alarm style)
void playFireAlarm() {
  for (uint8_t group = 0; group < 3; group++) {
    for (uint8_t beep = 0; beep < 3; beep++) {
      tone(BUZZER_PIN, 3000, 100);
      delay(150);
    }
    noTone(BUZZER_PIN);
    delay(1200);
  }
}

// Ohrozeni: pomala civilni sirena, plynuly sweep nahoru a dolu / slow civil-defense sweep up and down
void playAirRaid() {
  for (uint8_t cycle = 0; cycle < 2; cycle++) {
    for (uint16_t f = 300; f <= 900; f += 5) {
      tone(BUZZER_PIN, f);
      delay(8);
    }
    for (uint16_t f = 900; f >= 300; f -= 5) {
      tone(BUZZER_PIN, f);
      delay(8);
    }
  }
  noTone(BUZZER_PIN);
}

// Red Alert (Star Trek TNG): opakovany rychly "whoop" nahoru / repeated fast upward whoop
void playRedAlert() {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint16_t f = 300; f <= 1200; f += 45) {
      tone(BUZZER_PIN, f);
      delay(5);
    }
    noTone(BUZZER_PIN);
    delay(120);
  }
}

// SOS v Morseove abecede: ... --- ... / SOS in Morse code: ... --- ...
void playSOS() {
  const uint16_t FREQ = 1000;
  const uint16_t DOT = 150;
  const uint16_t DASH = 450;
  const uint16_t GAP = 150;
  const uint16_t LETTER_GAP = 450;

  for (uint8_t repeat = 0; repeat < 2; repeat++) {
    for (uint8_t i = 0; i < 3; i++) { tone(BUZZER_PIN, FREQ, DOT); delay(DOT + GAP); } // S
    delay(LETTER_GAP);
    for (uint8_t i = 0; i < 3; i++) { tone(BUZZER_PIN, FREQ, DASH); delay(DASH + GAP); } // O
    delay(LETTER_GAP);
    for (uint8_t i = 0; i < 3; i++) { tone(BUZZER_PIN, FREQ, DOT); delay(DOT + GAP); } // S
    delay(1000);
  }
  noTone(BUZZER_PIN);
}

// ---------------------------------------------------------------------------
// Tabulka ukazek (funkcni ukazatele v PROGMEM) / Demo table (function pointers in PROGMEM)
// ---------------------------------------------------------------------------
typedef void (*MelodyFunc)();

const MelodyFunc MELODIES[MELODY_COUNT] PROGMEM = {
  playFirefighters,
  playPolice,
  playAmbulance,
  playFireAlarm,
  playAirRaid,
  playRedAlert,
  playImperialMarch,
  playSuperMario,
  playTetris,
  playMissionImpossible,
  playOdeToJoy,
  playSOS,
};

void playSelected(uint8_t idx) {
  printMelodyHeader(idx);
  MelodyFunc f = (MelodyFunc)pgm_read_ptr(&MELODIES[idx]);
  f();
  // Zahodit pozadavky vznikle behem prehravani, cekame az na novy stisk.
  // Discard requests raised during playback, wait for a fresh press.
  nextRequested = false;
  prevRequested = false;
  printBoth(F("Ukazka dokoncena. Cekam na stisk tlacitka."), F("Demo finished. Waiting for a button press."));
}

// ---------------------------------------------------------------------------
// Setup / Loop
// ---------------------------------------------------------------------------
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PREV_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_NEXT_PIN), isrNext, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PREV_PIN), isrPrev, FALLING);

  Serial.begin(9600);
  printBoth(F("=== Ukazky melodii pro pasivni buzzer ==="), F("=== Passive buzzer melody demos ==="));
  printBoth(F("Tlacitko na D2 = Dalsi ukazka"), F("Button on D2 = Next demo"));
  printBoth(F("Tlacitko na D3 = Predchozi ukazka"), F("Button on D3 = Previous demo"));
  printBoth(F("Cekam na stisk tlacitka..."), F("Waiting for a button press..."));
}

void loop() {
  if (nextRequested) {
    nextRequested = false;
    printBoth(F("Tlacitko: Dalsi"), F("Button: Next"));
    currentMelody = (currentMelody + 1) % MELODY_COUNT;
    playSelected(currentMelody);
  }

  if (prevRequested) {
    prevRequested = false;
    printBoth(F("Tlacitko: Predchozi"), F("Button: Previous"));
    currentMelody = (currentMelody - 1 + MELODY_COUNT) % MELODY_COUNT;
    playSelected(currentMelody);
  }
}
