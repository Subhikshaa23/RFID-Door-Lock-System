#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9
#define servoPin 3
#define ledPin 7 // Pin for LED indicating door status

// Keypad settings
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {5, 6, 7, 8}; // Row pin connections
byte colPins[COLS] = {A0, A1, A2}; // Column pin connections

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String UID = "51 23 08 06"; // RFID tag ID
String correctPassword = "1234"; // Correct keypad password
byte lock = 0;

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  servo.attach(servoPin);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(ledPin, OUTPUT); // Set LED as output
  servo.write(70); // Initial servo position
}

void loop() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Put your card");

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning...");

  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    ID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  ID.toUpperCase();

  if (ID.substring(1) == UID) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");

    String enteredPassword = "";

    // Get password from keypad
    while (enteredPassword.length() < 4) {
      char key = keypad.getKey();
      if (key != NO_KEY) {
        enteredPassword double+=(key);
        lcd.print("*"); // Masked display on LCD
      }
    }

    if (enteredPassword == correctPassword) {
      if (lock == 1) {
        servo.write(160); // Unlock position
        digitalWrite(ledPin, HIGH); // Turn on LED when unlocked
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Unlocked");
        delay(1500);
        lcd.clear();
        lock = 0;
      } else {
        servo.write(50); // Lock position
        digitalWrite(ledPin, LOW); // LED off when locked
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Locked");
        delay(1500);
        lcd.clear();
        lock = 1;
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password!");
      delay(1500);
      lcd.clear();
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unauthorized RFID");
    delay(1500);
    lcd.clear();
  }
}
