//Ferramentas > Placa > Gerenciador de Placa > Pesquisar por ESP8266 e instalar
//Ferramentas > Placa > selecionar ESP8266Modules
//Arquivos > Preferencias > URLs adicionais e gerenciadores de placa : http://arduino.esp8266.com/package_esp8266com_index.json

//Todos os links do Github devem ser baixados como .ZIP
//Após baixar bibliotecas Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP 
//Realizar este procedimento para todas as bibliotecas.

//Reiniciar o Arduino

//Biblioteca wifi do nodeMCUESP8266
#include <ESP8266WiFi.h>

//https://www.arduino.cc/en/Reference/SPI
#include <SPI.h>

//Biblioteca do RFID 
//https://github.com/miguelbalboa/rfid
#include <MFRC522.h>

//https://www.arduino.cc/en/Reference/Wire
#include <Wire.h> 

//Biblioteca do display LCD
//https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <LiquidCrystal_I2C.h>

//Biblioteca do clientMQTT
//http://pubsubclient.knolleary.net/api.html
//https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>


//Pinos do RFID
#define RST_PIN  15 
#define SS_PIN  2  

WiFiClient espClient;

PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27,16,2);

MFRC522 mfrc522(SS_PIN, RST_PIN);

//parametros WIFI.
const char* ssid = "uehara";
const char* password = "naoentre";

//parametros MQTT.
const char* mqtt_server = "m12.cloudmqtt.com";
const int mqtt_port = 15213;

void setup() {
  Serial.begin(9600);
  
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  
  client.setCallback(callback);
  
  SPI.begin();           
  
  mfrc522.PCD_Init();
  
  Wire.begin(4, 5); 

  lcd.begin();  

  printLCD("Passe o Cartao!");  
}

//Método de conexão com rede WIFI
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando com ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereco IP : ");
  Serial.println(WiFi.localIP());
}

//Método de conexão com servidor MQTT
void conectMqtt() {
  while (!client.connected()) {    
    Serial.print("Conectando ao MQTT ...");    
    
    //Parametros são nodeMCUClient, usuárioMQTT, senhaMQTT
    if (client.connect("ESP8266Client","btqswnsx","KpPhAf77eFeQ")) {
      Serial.println("Conectado");
      //Inscrevendo-se no tópico retorno.
      client.subscribe("retorno");
    } else {
      Serial.print("Falha, rc=");      
      Serial.print(client.state());      
      Serial.println(" Tentando novamente em 5 segundos");      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//Método que recebe os retornos do tópico retorno
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Messagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  String mensagem = "";
  //Conversão da mensagem recebidade de byte pra String
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.println(mensagem);
  Serial.println();
  
  //Chamada ao método que controla o acesso
  verificaAcesso(mensagem);
}

//Método de controle de acesso.
void verificaAcesso(String mensagem){
  //Estrutura da mensagem recebida 
  // ENTRANDO/Nome_do_dono_do_cartao
  // SAINDO/Nome_do_dono_do_cartao
  // FALSE 

  if ( mensagem.substring(0,8) == "ENTRANDO" ){
    printLCD("BEM VINDO");
    delay(1000);
    printLCD(mensagem.substring(9));
    delay(1000);
  }else if (mensagem.substring(0,6) == "SAINDO" ){
    printLCD("ATE MAIS");
    delay(1000);
    printLCD(mensagem.substring(7));
    delay(1000);
  }else{
    printLCD("Acesso negado!");
    delay(1000);
  }  
  delay(1000);
  printLCD("Passe o Cartao!");
  return;
}

//Método utilitário para print no display LCD 
void printLCD(String mensagem){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Infra-Hardware");
    lcd.setCursor(0,1);
    lcd.print(mensagem);
}

//Método de envio do id do cartão lido pra fila acesso
void sendMessage(MFRC522 mfrc522){
  printLCD("Lendo Cartao");
  char rfidstr[15];
  char s[100];
  for (int i = 0; i < mfrc522.uid.size; i++){
    
    //Conversão de byte pra Hexadecimal
    sprintf(s,"%x",mfrc522.uid.uidByte[i]);
    
    strcat( &rfidstr[i] , s);
  }
  Serial.print("Card ID : ");
  Serial.print(rfidstr);

  //Publicando na fila acesso o id do cartão lido
  client.publish("acesso", rfidstr);  
  
  Serial.println();
  printLCD("Verificando...");

  //Voltando pro loop
  return;
}

void loop() {  
  //Verificando Status do ClientMQTT
  if (!client.connected()) {
    conectMqtt();
  }
  client.loop();

  //Verificando existencia do card no leitor
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(1000);
    return;
  }

  //Verificando Leitura do card
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(1000);
    return;
  }

  sendMessage(mfrc522);
    
}
