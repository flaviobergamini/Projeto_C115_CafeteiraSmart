#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

// SSID e Password para o modo AP
const char *ssid_ap = "SWS_Module";
const char *password_ap = "12345678";
const char *ssid_new;
const char *pass_new;
String serverAddress_http = "http://10.0.0.103";

// Update these with values suitable for your network.

const char* ssid = "MS-DOS";
const char* password = "F80036562945";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  digitalWrite(D0, LOW);
  digitalWrite(D1, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char number;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    number = (char)payload[i];
    Serial.print(number);
    if(number == '1')
      digitalWrite(D2, HIGH);
    else
      digitalWrite(D2, LOW);
  }
  Serial.println();

  // Find out how many bottles we should generate lyrics for
  String topicStr(topic);
  int bottleCount = 0; // assume no bottles unless we correctly parse a value from the topic
  if (topicStr.indexOf('/') >= 0) {
    // The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/')+1);
    // Now see if there's a number of bottles after the '/'
    bottleCount = topicStr.toInt();
  }

  if (bottleCount > 0) {
    // Work out how big our resulting message will be
    int msgLen = 0;
    for (int i = bottleCount; i > 0; i--) {
      String numBottles(i);
      msgLen += 2*numBottles.length();
      if (i == 1) {
        msgLen += 2*String(" green bottle, standing on the wall\n").length();
      } else {
        msgLen += 2*String(" green bottles, standing on the wall\n").length();
      }
      msgLen += String("And if one green bottle should accidentally fall\nThere'll be ").length();
      switch (i) {
      case 1:
        msgLen += String("no green bottles, standing on the wall\n\n").length();
        break;
      case 2:
        msgLen += String("1 green bottle, standing on the wall\n\n").length();
        break;
      default:
        numBottles = i-1;
        msgLen += numBottles.length();
        msgLen += String(" green bottles, standing on the wall\n\n").length();
        break;
      };
    }
  
    // Now we can start to publish the message
    client.beginPublish("greenBottles/lyrics", msgLen, false);
    for (int i = bottleCount; i > 0; i--) {
      for (int j = 0; j < 2; j++) {
        client.print(i);
        if (i == 1) {
          client.print(" green bottle, standing on the wall\n");
        } else {
          client.print(" green bottles, standing on the wall\n");
        }
      }
      client.print("And if one green bottle should accidentally fall\nThere'll be ");
      switch (i) {
      case 1:
        client.print("no green bottles, standing on the wall\n\n");
        break;
      case 2:
        client.print("1 green bottle, standing on the wall\n\n");
        break;
      default:
        client.print(i-1);
        client.print(" green bottles, standing on the wall\n\n");
        break;
      };
    }
    // Now we're done!
    client.endPublish();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("greenBottles/#");
      client.subscribe("casaFHMB/L1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D0,OUTPUT);         // WI-FI desconectado
  pinMode(D1,OUTPUT);         // WI-FI Conectado
  pinMode(D2,OUTPUT);         // Cafeteira
  pinMode(D7, INPUT);         // Botão de reset da memória EEPROM

  digitalWrite(D0,LOW);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);

  Serial.begin(115200);
  EEPROM.begin(1024);
  
  byte value = EEPROM.read(0);
  EEPROM.end();
  if(digitalRead(D7) == LOW){
    eraseEEPROM();
    Serial.print("eraseEEPROM");
  }
  if(value == 0)
  {
    Serial.print("Value = 0");
    delay(500);
    //modeAP();
   }
   else
   {
   ReadEEPROM(ssid_ap, password_ap); 
   }
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// --------------- Trabalhando com a memória EEPROM ---------------
void eraseEEPROM() {
  EEPROM.begin(1024);
  for (int i = 0; i < 1024; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

void WriteEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();
 
  Serial.println("writing eeprom ssid:");
          for (int i = 0; i < ssidlen; ++i)
            {
              EEPROM.write(i, ssid[i]);
              Serial.print("Wrote: ");
              Serial.println(ssid[i]); 
            }

  Serial.println("writing eeprom password:");
          for (int i = 0; i < passlen; ++i)
            {
              EEPROM.write((i+ssidlen), password[i]);
              Serial.print("Wrote: ");
              Serial.println(password[i]); 
            }
  EEPROM.end();
}

void ReadEEPROM(String ssid, String password)
{
  EEPROM.begin(1024);
  int ssidlen = ssid.length();
  int passlen = password.length();

  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < ssidlen; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    //esid.trim();
  Serial.println(esid.length());
  Serial.print("SSID saida: ");
  Serial.println(esid);
  
  Serial.println("\n");
  
  Serial.println("Lendo password EEPROM");
  String passq;
  for (int i = (ssidlen); i < (ssidlen+passlen); ++i)
  {
    passq +=char(EEPROM.read(i));
  }
  Serial.println("Tamanho da password: ");
  Serial.println(passq.length());
  Serial.println("Password saida: ");
  Serial.println(passq);
  
  ssid_new = esid.c_str();
  pass_new = passq.c_str();
  EEPROM.end();
}
