#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// SSID e Password para o modo AP
const char *ssid_ap = "SmartCafe";
const char *password_ap = "12345678";
const char *ssid_new;
const char *pass_new;

// Update these with values suitable for your network.

const char* ssid = "MS-DOS";
const char* password = "F80036562945";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
ESP8266WebServer server(80);
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void WriteEEPROM(String ssid, String password);
void ReadEEPROM();
void eraseEEPROM();

void setup_wifi(String ssid, String password) {
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
  // Gravando dados na eeprom
  WriteEEPROM(ssid, password);

  // Iniciando broker MQTT
  while(true){
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
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
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D0,OUTPUT);               // WI-FI desconectado
  pinMode(D1,OUTPUT);               // WI-FI Conectado
  pinMode(D2,OUTPUT);               // Cafeteira
  pinMode(D7, INPUT);               // Botão de reset da memória EEPROM

  digitalWrite(D0,LOW);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);

  Serial.begin(115200);
  EEPROM.begin(1024);
  
  byte value = EEPROM.read(0);
  EEPROM.end();
  Serial.println("");
  if(digitalRead(D7) == LOW){
    eraseEEPROM();
    Serial.println("eraseEEPROM");
  }
  if(value == 0)
  {
    Serial.println("Value = 0");
    delay(500);
    modeAP();
   }
   else
   {
   ReadEEPROM(); 
   Serial.println("*********** Tentando Conexao ***********");
   setup_wifi(String(ssid_new), String(pass_new));
   }
  
  //setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  server.handleClient();
  /*
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); */
}

// ----------------- Criando rede Access Point (AP) para configuração do Wi-Fi do modo station -----------------

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
 
<h2>Cafeteira Smart<h2>
<h3> C115 - 2021</h3>
 
<form action="/action_page">
  SSID:<br>
  <input type="text" name="SSID" value="">
  <br>
  Password:<br>
  <input type="password" name="Password" value="">
  <br><br>
  <input type="submit" value="Submit">
</form> 
 
</body>
</html>
)=====";

void handleRoot() {
 String s = MAIN_page;                                           //lendo HTML
 server.send(200, "text/html", s);                               //enviando HTML para o client
}

void handleForm() {
   digitalWrite(D0, HIGH);
   digitalWrite(D1, LOW);
   String ssid_Station = server.arg("SSID"); 
   String pass_Station = server.arg("Password"); 
   
   ssid_new = ssid_Station.c_str();
   pass_new = pass_Station.c_str();
   
   Serial.print("SSID: ");
   Serial.println(ssid_Station);
   
   Serial.print("Password: ");
   Serial.println(pass_Station);
   Serial.print("------------- \n");
   Serial.println(ssid_new);
   
   String s = "<a href='/'> Go Back </a>";
   server.send(200, "text/html", s); // Enviando página Web
   
   for (uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] WAIT %d...\n", t);
      Serial.flush();
      delay(800);
    }
    WiFi.mode(WIFI_STA);
    setup_wifi(ssid_new, pass_new);
    //WiFi.begin(ssid_new, pass_new);
    //delay(500);
    
    /*if ((WiFi.status() == WL_CONNECTED)) 
    {
      digitalWrite(D0, LOW);
      digitalWrite(D1, HIGH);
      
      Serial.print("Conectado \n");
      WriteEEPROM(ssid_new, pass_new);
      
      // ---------Iniciando Serviço do MQTT---------
      while(true){
        if (!client.connected()) {
          reconnect();
        }
        client.loop();
      }
      // -------------------------------------------
    }
    else{
      Serial.print("Não conectado \n");
      ESP.reset();
      modeAP();
    } */
}

void modeAP()
{
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  delay(1000);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  
  WiFi.softAP(ssid_ap, password_ap);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/action_page", handleForm); //form action is handled here
  server.begin();
  Serial.println("HTTP server started");
}

// ---------------------------- Trabalhando com a memória EEPROM ----------------------------
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
  EEPROM.write(ssidlen, '|');
  Serial.println("writing eeprom password:");
  for (int i = 0; i < passlen; ++i)
  {
    EEPROM.write((i+ssidlen+1), password[i]);
    Serial.print("Wrote: ");
    Serial.println(password[i]); 
  }
  EEPROM.end();
}

void ReadEEPROM()
{
  EEPROM.begin(1024);
  Serial.println("Lendo EEPROM ssid");
  
  bool ssidBool = false;
  String esid = "";
  String passq = "";
  for(int i = 0; i < EEPROM.length(); i++){
    if(char(EEPROM.read(i)) == '|')
      ssidBool = true;
    if(char(EEPROM.read(i)) != '|' and ssidBool == false){
        esid += char(EEPROM.read(i));
    }
    else if(char(EEPROM.read(i)) != '|' and ssidBool == true){
        passq +=char(EEPROM.read(i));
   }
  }
  
  //int ssidlen = ssid.length();
  //int passlen = password.length();

  //Serial.println("Reading EEPROM ssid");
  //String esid;
  /*for (int i = 0; i < ssidlen; ++i)
    {
      esid += char(EEPROM.read(i));
    }*/
    //esid.trim();
  //Serial.println(esid.length());
  Serial.print("SSID saida: ");
  Serial.println(esid);
  
  Serial.println("\n");
  
  //Serial.println("Lendo password EEPROM");
  //String passq;
  /*for (int i = (ssidlen); i < (ssidlen+passlen); ++i)
  {
    passq +=char(EEPROM.read(i));
  }
  Serial.println("Tamanho da password: ");
  Serial.println(passq.length());*/
  Serial.print("Password saida: ");
  Serial.println(passq);
  
  ssid_new = esid.c_str();
  pass_new = passq.c_str();
  EEPROM.end();
}
