// librerias
#include <ArduinoJson.h>
#include "DHT.h"
#include <RTCZero.h>

// constantes
#define FREQ 115200
#define RXD2 16
#define TXD2 17
#define SEC 1000
#define NUMSG 4
#define MaxBuffer 400

// Constantes de librerias
#define DHTPIN 3
#define DHTTYPE DHT11
#define PIN_ATN 10
#define awakeTime 1


// constantes de deepsleep
const byte alarmSeconds = 10;
const byte alarmMinutes = 0;
const byte alarmHours = 0;

volatile bool alarmFlag = false; // bandera de la alarma

// variables globales
const byte MINUTE = 60*SEC;

String      MyLoraID      = "2";
const char* LORAKEY       = "antena1"; 
String      LORA_RECEIVER =  "3";         // id de la antena que recibira mi mensaje se cambia dependiendo de la arquitectura de comunicacion
int         HumedadPin    =  A4;
int         LuxPin        =  A5;
String      Buffer        =  "";
String      message       =  "";

int sizeDatas = 0;

DHT dht(DHTPIN, DHTTYPE);
RTCZero zerortc;

const char* messageTest = "{\"T\":20,\"U\":20,\"H\":20,\"L\":20}";

const int numNodes = 3;

char BufferJson[MaxBuffer*(numNodes - 2)];



// metodos
void buildMessage();
void sendMsg();

typedef struct{
  String arr[numNodes - 2];
}sensorReturn;

sensorReturn Values(String);

void setup() {

  dht.begin();

  delay(3000);

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(FREQ);
  
  Serial.println("inicializada la comunicacion serial");
  delay(3000);
}

void loop() {
    sendMsg();
}

void sendMsg() {
  
  if (Serial2.available() > 0) {
    // llego la informacion
    if (alarmFlag == true) {
      
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(PIN_ATN, HIGH);

      delay(10000);
    
      StaticJsonDocument<MaxBuffer*(numNodes - 2)> doc;
      
      String datas = Serial2.readString();
  
      datas = datas.substring(
                 datas.indexOf("{"),
                 datas.indexOf("}") + 1
               );
  
      String order[]    = {"id","T","H","U","L"};
  
      for(int i = 0; i < 5; i++){
        
        sensorReturn valueArray = Values(datas);
    
        JsonArray space = doc.createNestedArray(order[i]);
  
        for(int j = 0; j< numNodes - 2 ; j++){
          space.add(valueArray.arr[j]);
        }
  
        if(order[i] == "id"){
           space.add(LORAKEY);
        }
  
        if(order[i] == "H"){
           space.add(dht.readHumidity());
        }
  
        if(order[i] == "T"){
           space.add(dht.readTemperature());
        }
  
        if(order[i] == "U"){
           space.add(map(analogRead(HumedadPin), 0, 1023, 100, 0));
        }
  
        if(order[i] == "L"){
           space.add(analogRead(LuxPin));
        }
        
        serializeJson(doc, BufferJson);
        
        datas = datas.substring(datas.indexOf(",") + 1, datas.length());
      }
  
      Buffer = String(BufferJson);
      sizeDatas = datas.length() + 1;
      
      Serial.println(Buffer);
      
      buildMessage();
  
      Serial.println(message);
      Serial2.println(message);
      
      alarmFlag = false;
      delay(3000);
        
    }

    sleep();
  }
}

sensorReturn Values(String datas){
  
    int initial = datas.indexOf('[') + 1;
    int Final = datas.indexOf(']');

    String values = datas.substring(initial,Final);
    
    int cont = 1;

    while(values.indexOf(',') != -1){
      cont++;
    }

    sensorReturn valuesArray;

    for(int i= 0; i < cont ; i++){
      if(values.indexOf(',') == -1){
        valuesArray.arr[i] = values; 
      }else{
        //String valuesAux = values.substring(0,values.indexOf(',') - 1)
        //values[i] = valuesAux;
        //values = values.substring(values.indexOf(',') + 1, values.length()) 
      }
    }

    return valuesArray;
}


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
