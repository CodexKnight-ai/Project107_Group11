#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize I2C LCD (address 0x27, size 16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define keypad rows and columns
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; // Row pins connected to A0–A3
byte colPins[COLS] = {2, 3, 4, 5};     // Column pins connected to 2–5

// Initialize keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LED pins for Braille dots
int braillePins[6] = {13, 12, 11, 10, 9, 8}; // LED pins connected to 13–8

// Braille patterns (example for numbers 1-9, letters A-D, and custom for #)
// Each array represents the ON/OFF state of 6 LEDs
const bool brailleMap[16][6] = {
  {1, 0, 0, 0, 0, 0}, // 1 (Dot 1)
  {1, 1, 0, 0, 0, 0}, // 2 (Dots 1, 2)
  {1, 0, 0, 1, 0, 0}, // 3 (Dots 1, 4)
  {1, 0, 0, 1, 1, 0}, // 4 (Dots 1, 4, 5)
  {1, 0, 0, 0, 1, 0}, // 5 (Dots 1, 5)
  {1, 1, 0, 1, 0, 0}, // 6 (Dots 1, 2, 4)
  {1, 1, 0, 1, 1, 0}, // 7 (Dots 1, 2, 4, 5)
  {1, 1, 0, 0, 1, 0}, // 8 (Dots 1, 2, 5)
  {0, 1, 0, 1, 0, 0}, // 9 (Dots 2, 4)
  {0, 1, 0, 1, 1, 0}, // 0 (Dots 2, 4, 5)
  {1, 0, 0, 0, 0, 0}, // A (Dots 1, 3)
  {1, 1, 0, 0, 0, 0}, // B (Dots 1, 2, 3)
  {1, 0, 0, 1, 0, 0}, // C (Dots 1, 3, 4)
  {1, 0, 0, 1, 1, 0}, // D (Dots 1, 3, 4, 5)
  {1, 0, 0, 0, 1, 0}, // E (Dots 1, 3, 5)
  {0, 0, 1, 1, 1, 1}  // Custom for # (all LEDs OFF)
};

void setup() {
  // Set LED pins as OUTPUT
  for (int i = 0; i < 6; i++) {
    pinMode(braillePins[i], OUTPUT);
    digitalWrite(braillePins[i], LOW); // Ensure all LEDs are OFF initially
  }
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Braille System");

  // Initialize Serial
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    // Display key on LCD
    lcd.clear();
    lcd.print("Key: ");
    lcd.print(key);

    // Convert key to Braille pattern index (0-15)
    int index = getIndex(key);

    if (index != -1) {
      Serial.print("Braille Index: ");
      Serial.println(index);
      lcd.setCursor(0, 1);
      lcd.print("Braille: ");
      displayBraille(index); // Display Braille pattern on LEDs
    } else {
      Serial.println("Invalid Key or No Braille Mapping.");
      lcd.setCursor(0, 1);
      lcd.print("No Mapping");
      clearBraille(); // Turn off all LEDs
    }
  }
}

// Map key to Braille pattern index
int getIndex(char key) {
  if (key >= '1' && key <= '9') return key - '1';        // Keys 1-9
  if (key == '0') return 9;                             // Key 0
  if (key >= 'A' && key <= 'D') return key - 'A' + 10;  // Keys A-D
  if (key == '#') return 15;                            // Key # maps to custom index
  return -1; // Invalid key
}

// Display Braille pattern on LEDs
void displayBraille(int index) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(braillePins[i], brailleMap[index][i] ? HIGH : LOW);
  }

  // Show Braille dots state on LCD
  for (int i = 0; i < 6; i++) {
    lcd.setCursor(9 + i, 1); // Start at position 9 on the second row
    lcd.print(brailleMap[index][i] ? "1" : "0");
  }
}

// Turn off all LEDs
void clearBraille() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(braillePins[i], LOW);
  }
}
