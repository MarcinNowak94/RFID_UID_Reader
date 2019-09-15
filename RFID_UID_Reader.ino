/* http://www.arduino.cc/en/Tutorial/LiquidCrystal */

#include <LiquidCrystal.h>            // LCD 
#include <MFRC522.h>                  // RFID RC522 Module
#include <SPI.h>                      // SPI

#define RST_PIN         9             // MFRC522 Reset pin
#define SS_PIN          10            // MFRC522 SlaveSelect pin

#define RS              8             // LCD     Register Select pin
#define Enable          6             // LCD     Enable pin
#define D4              5             // LCD     Digital 4 pin
#define D5              4             // LCD     Digital 5 pin
#define D6              3             // LCD     Digital 6 pin
#define D7              2             // LCD     Digital 7 pin

LiquidCrystal lcd(RS, Enable, D4, D5, D6, D7);  
MFRC522 mfrc522(SS_PIN, RST_PIN);     // Create MFRC522 instance

/* Typical pin layout used for RFID:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

void setup() {
  
  lcd.begin(16, 2);                                 // set up the LCD's number of columns and rows:
  lcd.print(F("RFID Reader"));
  lcd.setCursor(0, 1);
  lcd.print(F("Scan a card ..."));
  Serial.begin(9600);                               // Initialize serial communications with the PC
  while (!Serial);                                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                                      
  mfrc522.PCD_Init();                               
  mfrc522.PCD_DumpVersionToSerial();                // Show details of PCD - MFRC522 Card Reader details
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

int     screendelay=5000;
int     timetoread=1000;                            // Time buffer ensuring successful retrieval of all information from card
int     successRead;                                // Return code from the reader
byte    readCard[10];                               // Stores scanned ID read from RFID Module
String  CardID;                                     

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("RFID Reader"));
  lcd.setCursor(0, 1);
  lcd.print(F("Scan a card ..."));
  do {successRead = getID();}           // sets successRead to 1 when we get read from reader otherwise 0
    while (!successRead);               // the program will not go further while you not get a successful read
    
    // Dump debug info about the card; PICC_HaltA() is automatically called
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    CardID="";
    for (int i = 0; i < mfrc522.uid.size; i++) {  // for size of uid.size write uid.uidByte to readCard
    readCard[i] = mfrc522.uid.uidByte[i];
    CardID+=String(readCard[i], HEX);
    };
    
    Serial.println(CardID);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Cards UID:"));
      lcd.setCursor(0, 1);
      lcd.print(CardID);    
      delay(screendelay);
}

int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    Serial.print(F("."));
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    Serial.println(F("Reading serial ..."));
    return 0;
      }
  
  Serial.println(F("Scanning PICC's UID..."));
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("SCANNING"));
  lcd.setCursor(0, 1);
  lcd.print(F("PICC's UID..."));
  delay(timetoread);
  
  for (int i = 0; i < mfrc522.uid.size; i++) {  // for size of uid.size write uid.uidByte to readCard
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println(F(""));
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}
