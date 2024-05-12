#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9
#define servoPin 3
#define LED_PIN 4

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, A1, A2, A3}; //connect to the column pinouts of the keypad

String UID = "51 23 08 06";//Enter your card ID
byte lock = 0;
Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  servo.write(50);
  lcd.init();
  lcd.backlight();
  servo.attach(servoPin);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Put your card");

  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");
  Serial.print("NUID tag is :");
  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    lcd.print(".");
    ID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    ID.concat(String(rfid.uid.uidByte[i], HEX));
    delay(300);
  }
  ID.toUpperCase();

  if (ID.substring(1) == UID && lock == 0 ) {
     // Turn on the LED
    servo.write(50);
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locked");
    delay(1500);
    lcd.clear();
    lock = 1;
  } else if (ID.substring(1) == UID && lock == 1 ) {
    digitalWrite(LED_PIN, LOW); // Turn off the LED
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unlocked");
    delay(1500);
    lcd.clear();
    lock = 0;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong card!");
    delay(1500);
    lcd.clear();
  }
  // Ask for passcode
  lcd.setCursor(0, 0);
  lcd.print("Enter Passcode:");
  String enteredPasscode = "";
  while (enteredPasscode.length() < 4) {
    char key = keypad.getKey();
    if (key) {
      enteredPasscode += key;
      lcd.setCursor(0, 1);
      lcd.print(enteredPasscode);
      delay(500); // debounce delay
    }
  }
  // Check passcode
  if (enteredPasscode == "1234") { // Change "1234" to your desired passcode
    lcd.clear();
    lcd.print("Passcode OK");
    servo.write(270);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    lcd.clear();
    // Proceed with unlocking logic if needed
  } else {
    lcd.clear();
    lcd.print("Wrong passcode!");
    delay(1000);
    lcd.clear();
    // Proceed with locking logic or other actions if needed
  }
}
