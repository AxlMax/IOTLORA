// Developer Ing Axsel Garcia 15-11-2022

//librerias
#include <ArduinoJson.h>
#include "DHT.h"
#include <RTCZero.h>

// Constantes
#define FREQ 115200
#define SEC 1000
#define MaxBuffer 400

// Constantes de librerias
#define DHTPIN 3
#define DHTTYPE DHT11
#define PIN_ATN 10
#define awakeTime 1

// variables globales

const byte MINUTE = 60*SEC;

String LORA_RECEIVER  =  "2";         // id de la antena que recibira mi mensaje se cambia dependiendo de la arquitectura de comunicacion
String LORAKEY        =  "STest";     // topic de mosquitto debe cambiarse si es una nueva tarjeta
int HumedadPin        =  A4;
int LuxPin            =  A5;
String Buffer         =  "";
String message        =  "";

const byte alarmSeconds = 10;
const byte alarmMinutes = 0;
const byte alarmHours = 0;

volatile bool alarmFlag = false; // bandera de la alarma

// variables propias de librerias o esenciales en su uso
char BufferJson[MaxBuffer];
DHT dht(DHTPIN, DHTTYPE);
RTCZero zerortc;

// metodos
void buildBuffer();
void buildMessage();

//init
void setup() {
  Serial.begin(FREQ);
  Serial1.begin(FREQ);
  dht.begin();

  delay(3000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(PIN_ATN, OUTPUT);

  initAlarm();
}

void loop() {
  
  if (alarmFlag == true) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_ATN, HIGH);
    delay(10000);
    buildBuffer();
    buildMessage();
    Serial.println(message);
    Serial1.println(message);
    alarmFlag = false;  // Clear flag
    delay(3000);
  }

  sleep();
}
// construccion del buffer apartir de las mediciones de los sensores

void buildBuffer() {
  StaticJsonDocument<400> doc;

  JsonArray space = doc.createNestedArray("id");
  space.add(LORAKEY);
  serializeJson(doc, BufferJson);

  JsonArray space2 = doc.createNestedArray("H");
  space2.add(dht.readHumidity());
  serializeJson(doc, BufferJson);

  JsonArray space3 = doc.createNestedArray("T");
  space3.add(dht.readTemperature());
  serializeJson(doc, BufferJson);

  JsonArray space4 = doc.createNestedArray("U");
  space4.add(map(analogRead(HumedadPin), 0, 1023, 100, 0));
  serializeJson(doc, BufferJson);

  JsonArray space5 = doc.createNestedArray("L");
  space5.add(analogRead(LuxPin));
  serializeJson(doc, BufferJson);

  Buffer = String(BufferJson);
}
// construccion del mensaje apartir del buffer en formato json
void buildMessage() {
  String messageLen = (String)Buffer.length();
  message = "AT+SEND=" + LORA_RECEIVER + "," + messageLen + "," + Buffer;
}

void initAlarm(void){
  zerortc.begin(); 
  resetAlarm(); 
  zerortc.attachInterrupt(alarmMatch);   
}

void alarmMatch(void)
{
  alarmFlag = true; // Set flag
}

void resetAlarm(void) {
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
  byte day = 1;
  byte month = 1;
  byte year = 1;
  
  zerortc.setTime(hours, minutes, seconds);
  zerortc.setDate(day, month, year);

  zerortc.setAlarmTime(alarmHours, alarmMinutes, alarmSeconds);
  zerortc.enableAlarm(zerortc.MATCH_HHMMSS);
}

void sleep(void){
  
  resetAlarm();  
  
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_ATN, LOW);
  
  zerortc.standbyMode();
}
