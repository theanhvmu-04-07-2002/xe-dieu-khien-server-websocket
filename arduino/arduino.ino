#include <Wire.h>

#define cambientrong 3
//#define cambienngoai 8

#define congtacdong 6
#define congtacmo 7

#define pin_dongcua 8
#define pin_mocua 9

String tg="";
String tg2="";
String guidulieu="";


void setup() {
  
 Wire.begin(8);                /* join i2c bus with address 8 */
 
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 
 Serial.begin(9600);           /* start serial for debug */
 
 pinMode(pin_dongcua,OUTPUT);
 pinMode(pin_mocua,OUTPUT);
 
 pinMode(cambientrong,INPUT);
// pinMode(cambienngoai,INPUT);
 
 pinMode(congtacdong,INPUT_PULLUP);
 pinMode(congtacmo,INPUT_PULLUP);

 
}

void loop() {
  if(tg=="tudong"){
    Modetudong();
  }
  if(tg=="thucong"){
    Modethucong();
  }
 delay(500);
}

// Hàm nhận dữ liệu từ ESP
void receiveEvent(int howMany) {
 String nhandulieu="";
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    nhandulieu+=c;
  } 
 if(nhandulieu=="tudong"){
  tg="tudong";
 }
 if(nhandulieu=="thucong"){
  tg="thucong";
 }
 if(nhandulieu=="thucongmocua"){
  tg2="thucongmocua";
 }
 if(nhandulieu=="thucongdongcua"){
  tg2="thucongdongcua";
 }
}

// Hàm gửi dữ liệu cho ESP
void requestEvent() {
  int res=0;
if(guidulieu == "Cuadadong......"){
  //Wire.write(2);
  res=2;
}
if(guidulieu == "Cuadangdong...."){
  //Wire.write(3);
  res=3;
}
if(guidulieu == "Cuadangmo......"){
 // Wire.write(4);
  res=4;
}
if(guidulieu == "Cuamohoantoan.."){
 // Wire.write(5);
  res=5;
}
if(guidulieu == "CuadangmoTC...."){
 // Wire.write(6);
  res=6;
}
if(guidulieu == "CuamohoantoanTC"){
  //Wire.write(7);
  res=7;
}
if(guidulieu == "CuadadongTC...."){
//  Wire.write(8);
  res=8;
}
if(guidulieu == "CuadangdongTC.."){
//  Wire.write(9);
  res=9;
}
 // Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(res);  /* sends hello string */
//  Wire.endTransmission();    /* stop transmitting */  
 delay(1000);
}

void Modetudong(){
  Serial.println("Kich hoat mode tu dong");
 
 if(digitalRead(cambientrong)== LOW){
  
    if(digitalRead(congtacdong)==LOW){
      
      Serial.println("0 phát hiện cd,đã đóng");
      
      guidulieu="Cuadadong......";
      DungCua();
    }
    else{
      guidulieu="Cuadangdong....";
      Serial.println("0 phát hiện cd,đang đóng");
      DongCua();
    }
   
 }
 else{
  if(digitalRead(congtacmo)==HIGH){
    guidulieu="Cuadangmo......";
    Serial.println("phát hiện cd,đang mở");
    MoCua();
  }
  else{
    guidulieu="Cuamohoantoan..";
    Serial.println("phát hiện cd,đã mở");
    DungCua();
  }
  
 }
 
}
void Modethucong(){
  Serial.println("Kich hoat mode thu cong");
    if(tg2=="thucongmocua"){

      if(digitalRead(congtacmo)==HIGH){
        Serial.println("Cửa đang mở");
        guidulieu="CuadangmoTC....";
        MoCua();
      } 
      else{
        guidulieu="CuamohoantoanTC";
        DungCua();
      }
       
  }
  if(tg2=="thucongdongcua"){
  
    if(digitalRead(congtacdong)==LOW){
      guidulieu="CuadadongTC....";
       DungCua();
    }
    else{
       DongCua();
       Serial.println("Cửa đang đóng");
       guidulieu="CuadangdongTC..";
     } 
   
  }
     
}

void DongCua(){  
  digitalWrite(pin_dongcua,HIGH);
  digitalWrite(pin_mocua,LOW); 
}
void MoCua(){  
  digitalWrite(pin_dongcua,LOW);
  digitalWrite(pin_mocua,HIGH);  
}
void DungCua(){
  digitalWrite(pin_dongcua,LOW);
  digitalWrite(pin_mocua,LOW);  
}
