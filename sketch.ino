#include <LiquidCrystal.h>
#include <Keypad.h>

// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);

// Keypad pin mapping (ROWS: A0-A3, COLS: A4-A7)
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};  // R1-R4
byte colPins[COLS] = {A4, A5, A6, A7};  // C1-C4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Updated Button and Output Pins
const int weightButtonPin = 13;  // D13
const int startStopButtonPin = 12;  // D12
const int ledPin = 4;
const int buzzerPin = 5;

bool countdownRunning = false;
bool waitingForInput = false;
bool weightHeld = false;

unsigned long countdownStartMillis;
unsigned long totalSeconds = 0;

void setup() {
  pinMode(weightButtonPin, INPUT_PULLUP);
  pinMode(startStopButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Pomodoro Ready!");
}

void loop() {
  if (digitalRead(weightButtonPin) == LOW && !waitingForInput && !countdownRunning) {
    waitingForInput = true;
    weightHeld = true;
    lcd.clear();
    lcd.print("Enter Time MMSS:");
    totalSeconds = getTimeFromKeypad();
    if (totalSeconds > 0) {
      lcd.clear();
      lcd.print("Press Start Btn");
    } else {
      lcd.clear();
      lcd.print("Invalid Time");
      delay(2000);
      waitingForInput = false;
    }
  }

  if (digitalRead(startStopButtonPin) == LOW && waitingForInput && totalSeconds > 0) {
    countdownRunning = true;
    countdownStartMillis = millis();
    waitingForInput = false;
    lcd.clear();
  }

  if (countdownRunning) {
    unsigned long elapsed = (millis() - countdownStartMillis) / 1000;
    if (elapsed >= totalSeconds) {
      lcd.clear();
      lcd.print("Time's up!");
      tone(buzzerPin, 1000, 1000);
      digitalWrite(ledPin, HIGH);
      delay(2000);
      digitalWrite(ledPin, LOW);
      countdownRunning = false;
    } else {
      int remaining = totalSeconds - elapsed;
      int minutes = remaining / 60;
      int seconds = remaining % 60;
      lcd.setCursor(0, 0);
      lcd.print("Time Left:");
      lcd.setCursor(0, 1);
      lcd.print((minutes < 10 ? "0" : "") + String(minutes) + ":"
              + (seconds < 10 ? "0" : "") + String(seconds));
      delay(500);

      if (digitalRead(weightButtonPin) == HIGH) {
        alertEarlyLift();
        countdownRunning = false;
      }
    }
  }
}

unsigned long getTimeFromKeypad() {
  String input = "";
  lcd.setCursor(0, 1);

  while (input.length() < 4) {
    char key = keypad.getKey();
    if (key != NO_KEY && isDigit(key)) {
      input += key;
      lcd.print(key);
    }
  }

  int mins = input.substring(0, 2).toInt();
  int secs = input.substring(2).toInt();

  if (secs >= 60) return 0;
  return mins * 60 + secs;
}

void alertEarlyLift() {
  lcd.clear();
  lcd.print("Weight Lifted!");
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);
    delay(300);
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    delay(300);
  }
  lcd.clear();
  lcd.print("Reset Required");
  delay(2000);
}
