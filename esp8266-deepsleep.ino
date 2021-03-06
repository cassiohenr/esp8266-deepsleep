#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

uint32_t sleep_time_s = 2 * 1000000;                  //Equivale a 30min
 
ADC_MODE(ADC_VCC);                                           //vcc read
 
const char* ssid = "WIFI";                             //Definir o SSID da rede WiFi
const char* password = "SENHA";                     //Definir a senha da rede WiFi
 
WiFiUDP ntpUDP;
 
int16_t utc = -3;                                                         //UTC -3:00 Brazil
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);
 
String apiKey = "CHAVE";                                      //Colocar a API Key para escrita neste campo
const char* server = "api.thingspeak.com";                            //Ela é fornecida no canal que foi criado na aba API Keys
 
WiFiClient client;
 int valor=0;
 
void setup()
{
  Serial.begin(115200);                       //Configuração da UART
  delay(10);
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_STA);                       //usar apenas no primeiro upload.   
  WiFi.begin(ssid, password);                  //Inicia o WiFi
   
 while (WiFi.status() != WL_CONNECTED)        //Loop até conectar no WiFi
  {
    delay(500);
    Serial.print(".");
  }
 
  timeClient.begin();
  timeClient.update();
 
  Serial.println("");                              //Logs na porta serial
  Serial.println("WiFi conectado!");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}
 
void envia_dados(void)
{ 
  if (client.connect(server,80))                                //Inicia um client TCP para o envio dos dados
  {
    float vdd = ESP.getVcc() / 1024.0;
    
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(valor);
           //postStr +="&field2=";
           //postStr += String(t);
           postStr +="&field3=";
           postStr += String(vdd);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
 
  }
  client.stop();
}
 
void loop() 
{  
  
    valor=valor+1;  //somente testando
    envia_dados();
    delay(15000);
    valor=0; 
    envia_dados();
    delay(15000);
  
  
  ESP.deepSleep(sleep_time_s);
}
