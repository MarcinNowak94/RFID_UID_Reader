/* http://www.arduino.cc/en/Tutorial/LiquidCrystal */

#include <EEPROM.h>                 //Library To read and write PICC's UIDs from/to EEPROM
#include <LiquidCrystal.h>          //LCD 
#include <MFRC522.h>                //RFID RC522 Module
#include <SPI.h>                    //SPI

#define RST_PIN         9           //Reset pin
#define SS_PIN          10          //SlaveSelect pin

LiquidCrystal lcd(8, 6, 5, 4, 3, 2);  //Initializing LCD PINS as (RS,EN,D4,D5,D6,D7)
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
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("RFID Reader");
  lcd.setCursor(0, 1);
  lcd.print("Scan a card ...");
  Serial.begin(9600);                               // Initialize serial communications with the PC
  while (!Serial);                                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                                      // Init SPI bus
  mfrc522.PCD_Init();                               // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();                // Show details of PCD - MFRC522 Card Reader details
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

int     screendelay=5000;                              // Variable screen delay
int     successRead;                                  // Variable integer to keep if we have Successful Read from Reader
byte    readCard[6];                                  // Stores scanned ID read from RFID Module
String  CardID;

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Reader");
  lcd.setCursor(0, 1);
  lcd.print("Scan a card ...");
  do {successRead = getID();
      Serial.println(F("No card so far ..."));}           // sets successRead to 1 when we get read from reader otherwise 0
    while (!successRead);               //the program will not go further while you not get a successful read
    for ( int j = 0; j < 4; j++ ) {     
      EEPROM.write( j, readCard[j] );   // Write scanned PICC's UID to EEPROM, start from address 3
    }
    // Dump debug info about the card; PICC_HaltA() is automatically called
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

    CardID="";
    CardID+=String(readCard[0], HEX);
    CardID+=String(readCard[1], HEX);
    CardID+=String(readCard[2], HEX);
    CardID+=String(readCard[3], HEX);
    
    Serial.print("Card: ");
    Serial.print(readCard[0], HEX);
    Serial.print(readCard[1], HEX);
    Serial.print(readCard[2], HEX);
    Serial.print(readCard[3], HEX);

    Serial.println(CardID);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cards UID:");
      lcd.setCursor(0, 1);
      lcd.print(CardID);    
      delay(screendelay);
}

int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
    Serial.println(F("No card yet..."));
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return 0;
    Serial.println(F("Reading serial ..."));
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs

  Serial.println("Scanning PICC's UID.........");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SCANNING");
  lcd.setCursor(0, 1);
  lcd.print("PICC's UID...");
  delay(2000);
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}
