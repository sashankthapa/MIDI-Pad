const byte ROWS = 3;
const byte COLS = 5;

// Physical Pins
const byte rowPins[ROWS] = {7, 8, 9};
const byte colPins[COLS] = {2, 3, A0, 5, 6};

// Flipped Layout
const byte keymap[ROWS][COLS] = {
  {72, 71, 69, 67, 65},
  {64, 62, 60, 59, 57},
  {55, 53, 52, 50, 48}
};

bool rawState[ROWS][COLS];
bool stableState[ROWS][COLS];
unsigned long debounceTimer[ROWS][COLS];

const uint8_t DEBOUNCE_TIME = 5;
const byte VELOCITY = 127;

// ---------------------------
// MIDI Functions
// ---------------------------

void sendNoteOn(byte note) {
  Serial.write(0x90);   // Note On, Channel 1
  Serial.write(note);
  Serial.write(VELOCITY);
}

void sendNoteOff(byte note) {
  Serial.write(0x80);   // Note Off, Channel 1
  Serial.write(note);
  Serial.write((byte)0);
}

// ---------------------------

void setup() {

  Serial.begin(115200);

  // Columns are inputs with pullups
  for (byte c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  // Rows are outputs
  for (byte r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
}

void loop() {

  unsigned long now = millis();

  for (byte r = 0; r < ROWS; r++) {

    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(5);

    for (byte c = 0; c < COLS; c++) {

      bool reading = (digitalRead(colPins[c]) == LOW);

      if (reading != rawState[r][c]) {
        rawState[r][c] = reading;
        debounceTimer[r][c] = now;
      }

      if ((now - debounceTimer[r][c]) >= DEBOUNCE_TIME) {

        if (stableState[r][c] != rawState[r][c]) {

          stableState[r][c] = rawState[r][c];

          if (stableState[r][c]) {
            sendNoteOn(keymap[r][c]);
          } else {
            sendNoteOff(keymap[r][c]);
          }
        }
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }
}
