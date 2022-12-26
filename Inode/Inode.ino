#include <ArduinoJson.h>
#include "DHT.h"

#define FREQ 115200
#define RXD2 16
#define TXD2 17
#define SEC 1000
#define NUMSG 4

int sizeDatas = 0;

DHT dht(DHTPIN, DHTTYPE);

// Constantes de librerias
#define DHTPIN 3
#define DHTTYPE DHT11

const char* messageTest = "{\"T\":20,\"U\":20,\"H\":20,\"L\":20}";

const int numNodes = 3;

const unsigned int MaxBuffer = 400;

char BufferJson[MaxBuffer*(numNodes - 2)];

void sendMsg();

typedef struct{
  String arr[numNodes - 2];
}sensorReturn;

sensorReturn Values(String);

// contantes del sensor
const char* LORAKEY    = "antena1"; // como se encuentra registrada la antena en la base de datos
const char* myNumber   = "2";       // numero de id de la antena
const char  sendNumber = "3";       // numero de id de la antena a la que enviare la informacion    

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

    String Buffer = String(BufferJson);
    sizeDatas = datas.length() + 1;

    Serial.println(Buffer);

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
