
// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLgT6BiRPv"
#define BLYNK_DEVICE_NAME "RGB"

  #define BLYNK_FIRMWARE_VERSION        "0.1.6"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#include "DHT.h"

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#define REDPIN 5  //D1
#define GREENPIN 4  //D2
#define BLUEPIN 14 //D5
#define WATERMOTORPIN 12 //D6 

#define wifiLed   16   //D0

#define SoilPIN A0 //A0 

#define VPIN_1 V1
#define VPIN_2 V2
#define VPIN_3 V3
#define VPIN_4 V4
#define VPIN_5 V5
#define VPIN_8 V8
#define VPIN_7 V7

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include "BlynkEdgent.h"


int redvalue = 0;
int greenvalue = 0;
int bluevalue = 0;
boolean waterMotor_state = LOW;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_1);
  Blynk.syncVirtual(VPIN_2);
  Blynk.syncVirtual(VPIN_3);
  Blynk.syncVirtual(VPIN_7);
}

BLYNK_WRITE(VPIN_1) {
  redvalue = param.asInt();
}

BLYNK_WRITE(VPIN_2) {
  greenvalue = param.asInt();
}

BLYNK_WRITE(VPIN_3) {
  bluevalue = param.asInt();
}

BLYNK_WRITE(VPIN_7) {
  waterMotor_state = param.asInt();
  if(waterMotor_state==HIGH){
     digitalWrite(WATERMOTORPIN, LOW);
  }else {
    digitalWrite(WATERMOTORPIN, HIGH);
  }
}

int soil_moisture;


BlynkTimer timer1;
float h , t;


void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature();

  soil_moisture = map(analogRead(SoilPIN), 23, 1023, 0, 100); 

    if (isnan(h) || isnan(t)){
    Serial.println("Failed to read from sensor!");
    return;
  }
  Blynk.virtualWrite(VPIN_4, h);
  Blynk.virtualWrite(VPIN_5, t);
  Blynk.virtualWrite(VPIN_8, soil_moisture);
}



void setup()
{
  Serial.begin(115200);
  delay(100);

  pinMode(REDPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(WATERMOTORPIN, OUTPUT); 
  
  pinMode(DHTPIN, INPUT);

  pinMode(SoilPIN, INPUT);

  pinMode(wifiLed, OUTPUT);

  digitalWrite(WATERMOTORPIN, HIGH);

  BlynkEdgent.begin();

  Blynk.virtualWrite(VPIN_1, redvalue);
  Blynk.virtualWrite(VPIN_2, greenvalue);
  Blynk.virtualWrite(VPIN_3, bluevalue);
  Blynk.virtualWrite(VPIN_7, waterMotor_state);

  timer1.setInterval(2000L, sendSensor);
} 

void loop() {
  BlynkEdgent.run();
  RGB_color(redvalue, greenvalue, bluevalue);
  timer1.run();
  Check_Motor_Status; 

}

void Check_Motor_Status(){

      if(digitalRead(WATERMOTORPIN)==LOW){
         Blynk.virtualWrite(VPIN_7, HIGH);
         waterMotor_state = HIGH;
      }else{
        Blynk.virtualWrite(VPIN_7, LOW);
        waterMotor_state = LOW;
      }
}


void RGB_color(int r, int g, int b) {

  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
}
