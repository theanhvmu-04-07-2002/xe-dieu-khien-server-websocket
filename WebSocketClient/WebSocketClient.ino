#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>


#include <WebSocketsClient.h> //https://github.com/Links2004/arduinoWebSockets
unsigned long times;
WebSocketsClient webSocket;

const char* ssid = "NgoMinhHoang"; //Đổi thành wifi của bạn
const char* password = "hieu90hp89"; //Đổi pass luôn

const char* ip_host = "192.168.1.6"; //Đổi luôn IP host của PC nha
const uint16_t port = 3000; //Port thích đổi thì phải đổi ở server nữa

String Cambien="";
String Trangthai="";
String Chedo;
String requestServer="";
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Chedo=(char*)payload;  
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
      }
      break;
    case WStype_BIN:
      Serial.println(Chedo);     
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(D1,D2);
  times = millis(); 
  Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  webSocket.begin(ip_host, port);
  webSocket.onEvent(webSocketEvent);
}

void Guidulieu_Arduino(){
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(Chedo.c_str());  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */  
}
  
void Guidulieu_Server(){
  String str="";  
  Wire.requestFrom(8,15);
  if(Wire.available()){
    int str = Wire.read();     
    
  if(str== 2){
    requestServer="2";   
  }
  else if(str== 3){
     requestServer="3";
  }
  else if(str== 4){
    requestServer="4";
  }
  else if(str== 5){
   requestServer="5";
  }
  else if(str== 6){
   requestServer="6";
  }
  else if(str== 7){
   requestServer="7";
  }
  else if(str== 8){
    requestServer="8";
  }
  else if(str== 9){
   requestServer="9";  
  }
  
 //Serial.println(requestServer);    
 }

  
 if(requestServer != ""){
  if((unsigned long) (millis() - times) > 1000){
     webSocket.sendTXT(requestServer);
     requestServer = "";
      times = millis();
  }
    
 }
 
}            

void loop() {

  Guidulieu_Arduino();
  Guidulieu_Server();
  webSocket.loop();

}
