#include <Arduino.h>
#include <Wire.h>   // thư viện liên kết giao thức i2c
#include <ESP8266WiFi.h>  // thư hiện hỗ trợ cho esp8266
#include <ArduinoJson.h>  // thư viện hỗ trợ cho việc xử lý dữ liệu dưới dạng Json
#include <WebSocketsClient.h> //https://github.com/Links2004/arduinoWebSockets // thư viện giao tiếp kết nối webSocket
// Khai báo các thư viện cần thiết

// khai báo các biến
unsigned long times;   // khai báo biến times - kiểu số nguyên không âm 
WebSocketsClient webSocket; // tạo lớp websocketsclient có tên là webSocket

// Các biến khai báo kết nối với mạng internet 
const char* ssid = "6B"; //Đổi thành wifi của bạn - tên mạng
const char* password = "0796411671m"; //Đổi pass luôn -- mk mạng

const char* ip_host = "192.168.1.200"; // IP của thiết bị khởi chạy server khi kết nối mạng 
const uint16_t port = 3000; //Khai báo Port kết nối đến server (phải trùng với port mà server đang xử lý)
int receivedDataSize = 4; // Biến đếm do kích thước mảng dữ liệu nhận được từ Arduino (có 4 loại gồm: Trạng thái cảm biến, trạng thái xe, tốc độ xe, khoảng cách)

int cambien=0;
int trangthai=0;
int khoangcach=0;
int tocdo=0;
String Chedo;  // biến kiểu chuỗi
String requestServer="1";
String nhandulieu="";

//được gọi khi có sự kiện xảy ra trong giao thức WebSocket.
//Hàm này xử lý các sự kiện như kết nối, ngắt kết nối, và nhận dữ liệu từ server
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) { // Hàm được gọi khi có 
  Chedo=(char*)payload;   // Dữ liệu nhận được từ server sẽ gán cho biến Chedo
  switch (type) 
  { 
    case WStype_DISCONNECTED: // Trạng thái mất kết nối đến server
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: // Trạng thái mất kết nối đến server
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
      }
      break;
    case WStype_BIN:  // Trạng thái nhận dữ liệu (loại dữ liệu là kiểu nhị phân)
      Serial.println(Chedo);     
      break;
  }
}

void setup() 
{
  Serial.begin(9600);
  Wire.begin(D1,D2);  // Thiết lập giao tiếp I2C qua chân D1, D2 của ESP
  times = millis(); 
  Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid, password);    // Kết nối tới wifi với thông tin đã khai báo ở trên
  while (WiFi.status() != WL_CONNECTED) // Khi chưa kết nối được thì in ra "." sau mỗi 0.5 giây
  {
    delay(500);
    Serial.print(".");
  }

  // Khi đã kết nối thì chạy tiếp lệnh dưới đây
  Serial.println(WiFi.localIP());   // in ra địa chỉ IP kết nối
  webSocket.begin(ip_host, port);   // Khởi tạo kết nối WebSocket đến server với địa chỉ IP và cổng đã được định nghĩa trước
  webSocket.onEvent(webSocketEvent);  // Hàm xử lý các sự kiện liên quan đến giao tiếp qua WebSocket.
}

void Guidulieu_Arduino()
{
  Wire.beginTransmission(8); /* Mở truyền dữ liệu của kết nối I2C thông qua địa chỉ = 8 */
  Wire.write(Chedo.c_str());  /*Gửi dữ liệu dạng string */
  Wire.endTransmission();   /* dừng truyền dữ liệu */ 
}
  
void Guidulieu_Server(int howMany )
{
  // Tạo mảng kiểu byte. Lưu ý rằng, kích thước mảng này nên trùng với kích thước mảng gửi dữ liệu từ Arduino
  byte dataReceived[10];
  
   Wire.requestFrom(8,15); //Yêu cầu dữ liệu từ thiết bị ngoại vi có địa chỉ I2C là 8 và kích thước dữ liệu là 15 bytes. Kích thước dữ liệu có thể thay đổi phù hợp với lượng dữ liệu gửi về
   if (Wire.available())  // Kiểm tra xem có dữ liệu nhận được từ thiết bị ngoại vi không.
   {
    // Mục đích là dùng vòng for để duyệt lần lượt, mỗi lần lặp nhận 1 phần tử trong mảng dữ liệu, lặp cho đến khi nhận hết.
    // Thực tế việc nhận dữ liệu đúng hay không do người viết code.
    // Bản chất vòng lặp for duyệt với số lần là howMany, nếu giả sử mảng dữ liệu gửi lên kích thước = 4, trong khi đó biến howMany = 3
    // => Vòng lặp for chỉ duyệt 3 lần => chỉ lấy được 3 loại dữ liệu (trong khi kích thước dữ liệu gửi lên là 4)
   {
     for (int i = 0; i < howMany; i++) {
        dataReceived[i] = Wire.read(); // Đọc từng phần tử dữ liệu gửi lên
      }
       // Gán dữ liệu nhận được cho từng biến phù hợp
       khoangcach = dataReceived[0];
       cambien = dataReceived[1];
       trangthai = dataReceived[2];
       tocdo = dataReceived[3];  
   }  

    if(requestServer != "")   // Câu lệnh này không cần thiết, có thể  loại bỏ
    {
      if((unsigned long) (millis() - times) > 1000)
      { 

         StaticJsonDocument<200> jsonDocument;  // Tạo 1 đối tượng JSON tĩnh để lưu dữ liệu

        // Đặt giá trị cho các trường trong JSON
        jsonDocument["khoangcach"] = khoangcach;
        jsonDocument["cambien"] = cambien;
        jsonDocument["trangthai"] = trangthai;
        jsonDocument["tocdo"] = tocdo;

        // Chuyển đổi JSON thành chuỗi
        String jsonStr;
        serializeJson(jsonDocument, jsonStr);

        // Gửi chuỗi JSON lên server qua kết nối WebSockets
        webSocket.sendTXT(jsonStr);


       // webSocket.sendTXT(requestServer);        
        Serial.println(requestServer);
        Serial.print("khoang cach: ");
        Serial.print(khoangcach);
        Serial.println(" cm");
        Serial.print("Cam bien: ");
        Serial.println(cambien);
        Serial.print("Trang thai: ");
        Serial.println(trangthai);
        Serial.print("Toc do: ");
        Serial.println(tocdo);
        khoangcach = 0;
        cambien = 0;
        trangthai = 0;   // Gán lại giá trị các biến về 0
        times = millis();  // Đặt lại biến times để thực hiện lần đếm thời gian tiếp theo
      }        
   }     
}         

void loop() {
  Guidulieu_Arduino();
  Guidulieu_Server(receivedDataSize);  // Như đã đề cập, hàm này cần khai báo dữ liệu = số lượng dữ liệu đã gửi, biến receivedDataSize có giá trị = 4 ở đầu chương trình
  webSocket.loop();  // Hàm xử lý sự kiện và giao tiếp qua WebSocket. Nếu hàm delay() được gọi, hàm này sẽ không hoạt động
}

