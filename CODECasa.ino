#include <LiquidCrystal_I2C.h>

#include <Wire.h>

#include "DHT.h"

#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

#include <Wire.h>

#include <Keypad.h>

void RGB_color(int r, int g, int b);
void printInfo();
void checkSoilMoisture();
void SolarTracker();
void ChangeRGBprofile();
void beep(int note, int duration);
void EntrySound();

//Ultrassonic
int echoPin = 4;
int trigPin = 3;


int counterLed = 0;

//RGB
int redPin = 35;
int bluePin = 36;
int greenPin = 34;


//Gás
#define GasPin A1
int InputGAS = 0;

#define BuzPin 7

int ServoPortaoPin = 5;
int angleServClosed = 180;
int angleServOpen = 90;

float duration = 0;
int distance = 0;

Servo servoPortao;

//Incializaçoes do ESTENDAL

#define ServoClothesPin 39
int angleServClothesRain = 180;
int angleServClothes = 0;
#define waterPin A2
boolean StatusPortao = false;
int waterValue = 0;

Servo servoClothes;

//DHT11
#define DHTTYPE DHT11
#define DHTPIN 6

DHT dht(DHTPIN, DHTTYPE);

//KeyPad

const byte ROWS = 4;
const byte COLS = 3;

byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28};

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Microfone
int MicPin = A3;
int MicValue = LOW;
int  MicCounter = 0;


//Motion Sensor
int PirPin = 8;
boolean pirVal = HIGH;
boolean PirState = LOW;

int LDRroof = A6;

int GasMotor = 9;
boolean GasStatus = LOW;

int rfindRstPin = 49;
int rfidSsPi = 53;

static volatile unsigned long time, elapsedRFID, prevRFID;
static volatile unsigned long elapsedDHT, prevDHT;
static volatile unsigned long elapsedGAS, prevGAS;
static volatile unsigned long elapsedPORTAO, prevPORTAO;
static volatile unsigned long elapsedKey, prevKey;
static volatile unsigned long elapsedPIR, prevPIR;
static volatile unsigned long elapsedClothes, prevClothes;
static volatile unsigned long elapsedLight, prevLight;
static volatile unsigned long elapsedMicro, prevMicro;


LiquidCrystal_I2C lcd(0x3F, 20, 4);

MFRC522 mfrc522(rfidSsPi, rfindRstPin);

const String knownKeyRfid = "E4 29 F7 39";

String atualKey = "";

String cardkey = "";

int profile = 0;
char prevProf = "*";

boolean isAuthenticated = false;

float Humidity = 0;
float temperature = 0;

//Leds Varanda

int LedVaranda = 40;

int lightValue = 0;

#define LDR1 A4
#define LDR2 A5

int tolerance = 5;
int solarPos = 90; //Posição Inicial
int valueLDR1 = 0;
int valueLDR2 = 0;
int error = 0;
int calibration = 230;

int ServoSolarPin = 12;
Servo servoSolar;


//Inicializações Soil Moisture Sensor

int map_low = 620;
int map_high = 310;

#define Soilsensor A0

int WaterMotor = 11;

int moisture = 0;
int Soilpercentage = 0;
boolean SoilStatus = LOW;

int counterSound = 0;
int durationSound = 0;
int note = 0;

int lightValuepercentage = 0;





/*-------------------------------------------*******************************************--------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  lcd.init();//initialize lcd screen
  lcd.backlight();// turn on the backlight
  lcd.setBacklight((uint8_t)1);// Turn on the blacklight
  lcd.setCursor(5, 0); // Second row
  lcd.print("BEM-VINDO!");// First row
  lcd.setCursor(5, 2); // Second row
  lcd.print("SMART HOME");

  dht.begin();

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(redPin,   OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  pinMode(PirPin, INPUT);

  pinMode(trigPin, OUTPUT);//Distance
  pinMode(echoPin, INPUT);//Distance

  pinMode(waterPin, INPUT);

  pinMode(GasPin, INPUT);

  pinMode(BuzPin, OUTPUT);

  pinMode(GasMotor, OUTPUT);

  pinMode(LedVaranda, OUTPUT);

  pinMode(LDRroof, INPUT);

  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);

  pinMode(WaterMotor, OUTPUT);
  pinMode(Soilsensor, INPUT);

  pinMode(MicPin, INPUT);

  digitalWrite(GasMotor, LOW);
  digitalWrite(WaterMotor, HIGH);

  servoClothes.attach(ServoClothesPin);
  servoClothes.write(angleServClothes);

  servoPortao.attach(ServoPortaoPin);
  servoPortao.write(180);

  servoSolar.attach(ServoSolarPin);
  servoPortao.write(solarPos);
}





void loop() {
  // put your main code here, to run repeatedly:

  printInfo();
  SolarTracker();
  checkSoilMoisture();
  ChangeRGBprofile();
  delay(500);


  time = millis();
  elapsedRFID = time - prevRFID;
  elapsedDHT = time - prevDHT;
  elapsedGAS = time - prevGAS;
  elapsedPORTAO = time - prevPORTAO;
  elapsedKey = time - prevKey;
  elapsedPIR = time - prevPIR;
  elapsedClothes = time - prevClothes;
  elapsedLight = time - prevLight;
  elapsedMicro = time - prevMicro;


  //RFID
  if (elapsedRFID > 500)
  {
    prevRFID = time;
    if (profile == 0)
    {
      if (mfrc522.PICC_IsNewCardPresent())
      {
        String atualKey = "";

        if (mfrc522.PICC_ReadCardSerial()) {
          Serial.println("Tag UID:");

          for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
            atualKey.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            atualKey.concat(String(mfrc522.uid.uidByte[i], HEX));
          }

          atualKey.toUpperCase();

          if (atualKey.substring(1) == knownKeyRfid) {

            profile = 1;
            counterSound = 1;

            lcd.clear();
            lcd.setCursor(5, 0);
            lcd.print("BEM-VINDO");

            // Second row
            lcd.setCursor(6, 1);
            lcd.print("EDUARDO");

            lcd.setCursor(10, 2);
            lcd.print("e");

            lcd.setCursor(6, 3);
            lcd.print("LEONARDO");
            isAuthenticated = true;
          } else {
            lcd.clear();
            lcd.setCursor(5, 0);
            lcd.print("SORRY");

            // Second row
            lcd.setCursor(2, 2);
            lcd.print("UNKNOWN KEY");

            RGB_color(255, 0, 0);

          }
          mfrc522.PICC_HaltA();
        }
      }
    }
  }

  if (profile != 0) {

    char customKey = customKeypad.getKey();
    //Serial.print(customKey);

    switch (customKey) {

      case '1':
        prevProf = customKey;
        profile = 1;
        break;
      case '2':
        prevProf = customKey;
        profile = 2;
        break;
      case '3':
        prevProf = customKey;
        profile = 3;
        break;
      case '4':
        prevProf = customKey;
        profile = 4;
        break;
      case '5':
        prevProf = customKey;
        profile = 5;
        break;
      case '*':
        prevProf = customKey;
        profile = 0;
        //Serial.print("Profile: ");
        //Serial.print(profile);
        isAuthenticated == false;

        lcd.clear();
        lcd.setCursor(4, 1);
        lcd.print("CASA BLOQUEADA");
        lcd.setCursor(2, 2);
        lcd.print(" PASSE O CARTAO");

        break;

      default:
        break;

    }
  }

  if (profile == 3) {
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("FELIZ");
    lcd.setCursor(6, 2);
    lcd.print(" NATAL");
    RGB_color(255, 0, 0);
    delay (100);
    RGB_color(0, 255, 0);
    delay(100);
  }

  if (profile == 5) {

    RGB_color(255, 165, 0);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("SOIL MOISTURE:");
    lcd.setCursor(14, 1);
    lcd.print(moisture);
    lcd.print("%");
  }

  //DHT11 SENSOR READS

  if (elapsedDHT >= 500)
  {
    temperature = (float)dht.readTemperature();
    Humidity = (float)dht.readHumidity();
  }
  if (profile == 2) {

    prevDHT = time;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURA: ");
    lcd.setCursor(6, 1);

    lcd.print(temperature);
    lcd.print(" C");
    //Serial.println(temperature);

    lcd.setCursor(0, 2);
    lcd.print("HUMIDADE:");
    lcd.setCursor(6, 3);
    lcd.print(Humidity);
    lcd.print("%");
    //Serial.println(Humidity);
  }

  //GAS SENSOR

  if (elapsedGAS >= 2000)
  {
    prevGAS = time;
    InputGAS = analogRead(GasPin);    //Obtem o Valor do Gas
    // Serial.println(InputGAS);

    if (InputGAS >= 400)
    {
      GasStatus = HIGH;
      while (GasStatus)
      {
        InputGAS = analogRead(GasPin);
        if (InputGAS < 400)
        {
          GasStatus = LOW;
        }
        RGB_color(255, 0, 0);

        tone(BuzPin, 500);

        digitalWrite(GasMotor, HIGH);

        delay(500);

        RGB_color(0, 0, 255);

        noTone(BuzPin);

        delay(500);
      }
      digitalWrite(GasMotor, LOW);
    }
  }

  // Micro

  if (elapsedMicro > 250) {

    prevMicro = time;

    if (profile != 0)

      MicValue = analogRead(MicPin);
    Serial.println(MicValue);

    if (MicValue > 550)
    {
      profile++;

      if (profile >= 5)
      {
        profile = 1;
      }
    }
  }


  //CAR GATE

  if ( elapsedPORTAO > 500)
  {
    prevPORTAO = time;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;      //Calculo da Distancia
    //Serial.println(distance);

    if (distance <= 15 && profile != 0 ) {

      prevPORTAO = time;

      servoPortao.attach(ServoPortaoPin);
      servoPortao.write(90);
      delay(15);
      RGB_color(130, 50, 70);
      delay(5000);
      servoPortao.attach(ServoPortaoPin);
      servoPortao.write(180);
      RGB_color(200, 0, 70);
      delay(15);
      StatusPortao == false;
    } else {
      servoPortao.attach(ServoPortaoPin);
      servoPortao.write(180);
    }

  }



  // Pir-Detetar Movimento!


  if (elapsedPIR > 300 && profile == 0) {

    prevPIR = time;

    if (profile == 0) {

      pirVal = digitalRead(PirPin);
      if (pirVal == HIGH) {        //MOTION DETECTED
        PirState = HIGH;
      }
    }

    if (PirState == HIGH) {

      while (PirState)
      {
        //Parar com o RFID
        RGB_color(255, 0, 0);
        tone(BuzPin, 1000);
        delay(200);
        RGB_color(0, 0, 0);
        noTone(BuzPin);
        lcd.clear();
        lcd.setCursor(5, 1);
        lcd.print("MOVIMENTO");
        lcd.setCursor(5, 2);
        lcd.print(" DETETADO");
        delay(200);
        if (mfrc522.PICC_IsNewCardPresent())
        {
          String atualKey = "";

          if (mfrc522.PICC_ReadCardSerial()) {

            for (byte i = 0; i < mfrc522.uid.size; i++) {
              Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
              Serial.print(mfrc522.uid.uidByte[i], HEX);
              atualKey.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
              atualKey.concat(String(mfrc522.uid.uidByte[i], HEX));
            }

            atualKey.toUpperCase();

            if (atualKey.substring(1) == knownKeyRfid) {

              PirState = LOW;
              profile = 1;
              counterSound = 1;

            }
            mfrc522.PICC_HaltA();
          }
        }
      }
    }
  }

  // ESTENDAL AUTOMÁTICO

  if (elapsedClothes >= 1000) {
    prevClothes = time;

    waterValue = analogRead(waterPin);

  }
  if (waterValue < 100) {
    servoClothes.attach(ServoClothesPin);
    servoClothes.write(angleServClothesRain);
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("   LEVE");
    lcd.setCursor(3, 2);
    lcd.print("GUARDA-CHUVA");
    delay(10);
  } else {
    servoClothes.attach(ServoClothesPin);
    servoClothes.write(angleServClothes);
  }

  // Leds Varanda

  if (elapsedLight > 1000) {

    prevLight = time;
    lightValue = analogRead(LDRroof);
    lightValuepercentage = map(lightValue, 0, 1023, 0, 100);
  }
  if (lightValue <= 500)
  {
    digitalWrite(LedVaranda, HIGH);
  } else {
    digitalWrite(LedVaranda, LOW);
  }

  if (profile == 4) {

    if (lightValue > 550) {
      RGB_color(255, 165, 0);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("ESTA UM DIA");
      lcd.setCursor(2, 1);
      lcd.print("SOALHEIRO");
      lcd.setCursor(0, 2);
      lcd.print("LUMONOSIDADE");
      lcd.setCursor(15, 2);
      lcd.print(lightValuepercentage);
      lcd.print("%");
    }

    else {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("BOA");
      lcd.setCursor(2, 1);
      lcd.print("NOITE");
      lcd.setCursor(0, 2);
      lcd.print("LUMONOSIDADE");
      lcd.setCursor(15, 2);
      lcd.print(lightValuepercentage);
      lcd.print("%");
    }
  }
}


void RGB_color(int r, int g, int b) {

  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}


void printInfo() {
  Serial.println();
  Serial.println("------------ New Round ------------");
  Serial.println();

  Serial.print("| Profile: ");
  Serial.print(profile);

  Serial.print("\n");

  Serial.print("| Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("| Water: ");
  Serial.println(waterValue);

  Serial.print("| Input GAS: ");
  Serial.println(InputGAS);

  Serial.print("| Humidity: ");
  Serial.println(Humidity);

  Serial.print("| Temperatura: ");
  Serial.println(temperature);

  Serial.print("| Luminosidade: ");
  Serial.println(lightValue);

  Serial.print("| SolarTracker Reads: ");
  Serial.print(valueLDR1);
  Serial.print(" | ");
  Serial.print(valueLDR2);

  Serial.print("\n");

  Serial.print("| Soil Reads: ");
  Serial.print(moisture);
 
  
  Serial.print("\n");

  Serial.print("|Micro Value: ");
  Serial.print(MicValue);

}


void SolarTracker() {

  valueLDR1 = calibration - 120 + analogRead(LDR1); // read the value of sensor 1
  valueLDR2 = analogRead(LDR2); // read the value of sensor 2

  if (valueLDR1 < 150 && valueLDR2 < 150) {     //Night Time

    while (solarPos <= 135)
    {
      solarPos++;
      servoSolar.write(solarPos);
      delay(2);
    }
  }

  error = valueLDR1 - valueLDR2;

  if (error > 15) {
    if (solarPos <= 135)
    {
      solarPos+=3;
      servoSolar.write(solarPos);
    }
  } else if (error < -15) {

    if (solarPos > 45);
    solarPos-=3;
    servoSolar.write(solarPos);
  }
}

// Microfone --> Muda de Peris
void CheckMicro() {

}




void checkSoilMoisture() {

  int moisture_1 = analogRead(Soilsensor);
  Serial.println(moisture_1);
  moisture = map(moisture_1, 23, 1023, 0, 100) + 15;
  

  if (moisture < 25)
  {
    SoilStatus == HIGH;
  }
  while (SoilStatus)
  {
    int moisture_1 = analogRead(Soilsensor);
    moisture = map(moisture_1, 23, 1023, 0, 100);
    digitalWrite( WaterMotor, LOW);
    if ( moisture >= 30) {
      SoilStatus == LOW;
    }
  } digitalWrite(WaterMotor, HIGH);
}


void ChangeRGBprofile() {

  switch (profile) {

    case 0:

      RGB_color(255, 0, 0);
      break;

    case 1:
      RGB_color(0, 255, 0);

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("BEM-VINDO!");

      lcd.setCursor(5, 2);
      lcd.print("SMART-HOME");
      break;

    case 2:
      RGB_color(0, 0, 255);
      break;

    default:
      break;
  }
}
