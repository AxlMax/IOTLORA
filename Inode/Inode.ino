#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define FREQ 115200
#define RXD2 16
#define TXD2 17
#define SEC 1000
#define NUMSG 4

int sizeDatas = 0;

// cambiar estos dos valores cada vez que se cambia de red
const char* ssid = "VHM Ingenieria_2.4";
const char* password = "vhm1ng3n13R14";

// broker de VHM

const char* mqttServer = "18.116.66.251";
const int mqttPort = 1883;

// credenciales admin
const char* mqttUser = "test";
const char* mqttPassword = "test";

// variables de librerias
WiFiClient espClient;
PubSubClient client(espClient);
DynamicJsonDocument doc(1024);

const char* messageTest = "{\"T\":20,\"U\":20,\"H\":20,\"L\":20}";
const char* topicTest   = "testingTopic";
const char* topicInit   = "LoraInit";

void conectWifi();
void sendMsg();

bool initflag;

void setup() {
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(FREQ);
  client.setServer(mqttServer, 1883);
  initflag = true;

  conectWifi();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  //client.publish(topicTest, messageTest); // probar la comunicacion mqtt broker

  // para iniciar el proceso de envio de datos en el servidor
  if (initflag) {
    client.publish(topicInit, "init");
    initflag = false;
  }


  for (int i = 0; i < NUMSG; i++) {
    // se envian tres mensajes
    sendMsg(); // para enviar los datos a mqtt
    delay(SEC);
  }


}

void conectWifi() {

  WiFi.begin(ssid, password);
  Serial.println("...................................");

  Serial.print("intentando conectarse a la red");
  while (WiFi.status() != WL_CONNECTED)
  { delay(500);
    Serial.print(".") ;
  }
  Serial.println("conectado");
}

void sendMsg() {
  
  if (Serial2.available() > 0) {
    // llego la informacion
    
    String datas = Serial2.readString();
    sizeDatas = datas.length() + 1;
    String cut = datas.substring(
                   datas.indexOf("{"),
                   datas.indexOf("}") + 1
                 );

    char charBuf[sizeDatas];

    cut.toCharArray(charBuf, sizeDatas);
    client.publish(topicTest, charBuf);

    Serial.print("data:");
    Serial.println(cut);

  }
}


void reconnect() {
  while (!client.connected()) {
    delay(2000);
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("conexion exitosa");
      delay(1000);
    } else {
      Serial.print("FALLO, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en un segundo");
      delay(1000);
    }
  }
}
