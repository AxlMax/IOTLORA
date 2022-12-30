#include <ArduinoJson.h>
#include <DHT.h>
 
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

String lora_band = "915000000"; //Banda de frecuencia (Hz)
String lora_networkid = "18";   //Identificación de la red Lora
String lora_address = "1";      //Dirección del módulo
String lora_RX_address = "2";   //Dirección del módulo receptor

String message = "";

void initAnthenna();
void BlinkBuild(int);
void SendLora();
void measureSensors();

void setup(){

  dht.begin();
  
  Serial.begin(115200);
  while (!Serial) continue;
  
  pinMode(LED_BUILTIN, OUTPUT);

  //initAnthenna();

}

void loop()
{
  BlinkBuild(1);
  //SendLora();
  measureSensors();  
}

void initAnthenna(){
  
  delay(1500);
  Serial.println("AT+BAND=" + lora_band);
  delay(500);
  Serial.println("AT+ADDRESS=" + lora_address);
  delay(500);
  Serial.println("AT+NETWORKID=" + lora_networkid);
  delay(1500);
}

void BlinkBuild(int dt){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(dt*500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(dt*500);
}

void SendLora(){
  Serial.println("AT+SEND="+ lora_RX_address +",2,ON");
}

void measureSensor(){
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  if (isnan(h) || isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");
}