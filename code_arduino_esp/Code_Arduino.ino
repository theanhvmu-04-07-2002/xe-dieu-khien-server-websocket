#include <Wire.h> // Thư viện hỗ trợ kết nối I2C
#include <ArduinoJson.h> // thư viện hỗ trợ xử lý dữ liệu json
#include <ArduinoJson.hpp> // thư viện hỗ trợ xử lý dữ liệu json

#define trigPin 4      // Chân Trig của cảm biến siêu âm kết nối với chân 4 trên Arduino - chân trig là chân output - chân đầu vào, chân nhập dl
#define echoPin 2     // Chân Echo của cảm biến siêu âm kết nối với chân 2 - chân echo là chân intput - chân đầu ra, chân xuất dl

#define cambiennhiet 5  // chân cắm output của cảm biến nhiệt kết nối với chân 5 trên arduino

int ena = 10;  //chân EnA điều chỉnh xung => điều khiển tốc độ động cơ - chỉ được cắm vào chân hỗ trợ băm xung (11 10 9 3 5 6 )
int enb = 3;   // chân EnB giống EnA

int in1 = 9; // khai báo chân cắm intput của L298N vào arduino ở chân số 9 8 7 6 
int in2 = 8; //
int in3 = 7; //
int in4 = 6; //

// các biến thiết lập bộ đếm thời gian để thay cho chức năng delay() trong chương trình
unsigned long previousTime = 0;  // Biến lưu trữ thời gian trước đó cho delay tương tự
const unsigned long interval = 1000;  // Khoảng thời gian mong muốn (ms) - thời gian trễ sẽ là 1 giây 

// nhóm các biến toàn cục liên quan đến xử lý lệnh, đặt trạng thái xe, tín hiệu cảm biến tốc độ xe
String tg = "";              // khai báo 1 dạng chuỗi ký tự dữ liệu với tên biến chuỗi là "tg"
int khoangcach = 0;         // Trạng thái khoảng cách ban đầu = 0 
int trangThaiHoatDong = 5 ; // Trang thái hoạt động mặc định = 5 <=> xe đang dừng
int cambien = 0;            // Trạng thái cảm biến = 0

int currentSpeed = 80; // Tốc độ hiện tại của động cơ (từ 0 đến 255)
int i; //Biến chạy phục vụ cho cơ chế tăng tốc của xe

void setup() { // hàm thiết lập và tạo các tài nguyên cho chương trình 
  
 Wire.begin(8); /*thiết lập giao tiếp I2C qua địa chỉ số 8 */

 /* Khởi tạo các hàm gửi và nhận dữ liệu */
 Wire.onReceive(receiveEvent); // nhận tín hiệu của giao thức I2C từ esp về arduino
 Wire.onRequest(requestEvent); // gửi tín hiệu  từ arduino sang esp - giao thức I2C
 
 Serial.begin(9600);   // Khởi tạo giao tiếp Serial với tốc bộ baud là 9600      
 
 // Thiết lập các chân input, output
 pinMode(cambiennhiet,INPUT);  // chân nhận dữ liệu cho arduino là chân số 5
 
 pinMode(trigPin,OUTPUT); // đặt cho chân trig vào chế độ xuất tín hiệu 
 pinMode(echoPin,INPUT);  // echo là nhập tín hiệu - arduino nhận tín hiệu cảm biến từ chân echo

  pinMode(in1, OUTPUT);   // đặt chân in1 = 9 là chân gửi tín hiệu từ arduino đến l298n
  pinMode(in2, OUTPUT);   // đặt chân in2 = 8 là chân gửi tín hiệu từ arduino đến l298n
  pinMode(in3, OUTPUT);  // đặt chân in3 = 7 là chân gửi tín hiệu từ arduino đến l298n
  pinMode(in4, OUTPUT);  // đặt chân in4 = 6 là chân gửi tín hiệu từ arduino đến l298n
  
  pinMode (ena, OUTPUT); // đặt vị trí gửi tín hiệu pwm từ arduino đến chân ena = 10 để kiểm soát tốc độ động cơ
  pinMode (enb, OUTPUT); // 
  idle();   // hàm dừng lại cho hệ thống động cơ DC
}

//Hàm idle thiết lập trạng thái dừng của xe
//Tương tự các hàm turnUp, turnDown, turnLeft, turnRight thiết lập cơ chế chuyển động của xe.
void idle() //Hàm idle thiết lập trạng thái dừng của xe - xe đứng yên
{
  // digitalwrite là để ghi giá trị điện áp high (có điện 5V) hoặc low(không có điện áp 0V)
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  i=0;
}
void turnUp() //Hàm turnUp thiết lập trạng thái tiến của xe
{ 
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  // dùng để cố định tốc độ chạy ban đầu khi bắt đầu điều khiển nút điều hướng
  analogWrite (enb, currentSpeed); //analogwrite dùng để thể hiện lệnh out dòng điện cho các chân pwm - chân xung nhịp trên arduino để truyền cho enA và enB
  analogWrite (ena, currentSpeed);
  i=currentSpeed; // là dùng cho tốc độ hiện tại
}

void turnDown() //Hàm turnDown thiết lập trạng thái lùi của xe
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite (enb, currentSpeed);
  analogWrite (ena, currentSpeed);
  i=currentSpeed;
}

void turnLeft() //Hàm turnLeft thiết lập trạng thái quay trái của xe
{
  analogWrite (ena, currentSpeed);
  analogWrite (enb, currentSpeed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  i=currentSpeed;
}

void turnRight() //Hàm turnRight thiết lập trạng thái quay phải của xe
{
  analogWrite (ena, currentSpeed);
  analogWrite (enb, currentSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  i=currentSpeed;
}

void speedUp() //Hàm speedUp  thiết lập trạng thái tăng tốc của xe
{
   for(i = currentSpeed; i<=255;i++)
   { 
      digitalWrite(in1, HIGH);                                          
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      analogWrite (ena, i);
      analogWrite (enb, i);
      if (shouldStopSpeedUp()) // Nếu lệnh nhận về không phải lệnh tăng tốc => thoát vòng for => ngắt trạng thái tăng tốc
      {
        i=currentSpeed; 
        break; // Thoát khỏi vòng lặp nếu cần dừng
      }
   delay(50);
  }
}

bool shouldStopSpeedUp() // mục đích là để kiểm tra dữ liệu nhận về , nếu không phải lệnh tăng tốc thì trả về true
{ 
  if(tg != "tangtoc")
  {
    return true;
  }
  return false;
}

// Hàm đọc dữ liệu từ cảm biến siêu âm
/* Bản chất việc đo khoảng cách vật thể bằng sóng âm là do sóng âm truyền trong không khi với vận tốc 343 m/s 
 *  khi sóng âm va vật cản, nó sẽ phản xạ lại. 
 Vì vậy ta chỉ cần tính thời gian từ lúc sóng âm phát ra đến khi dội trở lại nguồn âm => tính được khoảng cách  */
void Read_SRHC04() // Hàm đọc dữ liệu từ cảm biến siêu âm
{
  digitalWrite(trigPin, LOW); // Đặt chân trig xuống mức LOW
  delayMicroseconds(2); // độ trễ khoảng 2s số 2 này mặc định ko thay đổi được
  digitalWrite(trigPin, HIGH); // Khi ở mức HIGH, bắt đầu gửi tín hiệu siêu âm
  delayMicroseconds(10); // độ trễ khoảng 10s ko thể thay đổi được số này 
  digitalWrite(trigPin, LOW); // Sau khi gửi tín hiệu siêu âm, thiết lập lại mức LOW

  long duration = pulseIn(echoPin, HIGH); // Hàm pulseIn để do thời gian chân echo nhận tín hiệu phản xạ (long độ dài khoảng bảo lâu )
  khoangcach = duration * 0.0343 / 2;  // Chia 2 là do sóng âm đi 2 lần quãng đường (từ nguồn âm -> vật cản, sau đó lại từ vật cản -> nguồn âm) 0.0343 /2 ko thay đổi được đổi 
  // 343 m/s, cm / micro *100/1.000.000
}

void Read_SRHC505()  // Hàm đọc dữ liệu từ cảm biến chuyển động nhiệt
{
    if (digitalRead(cambiennhiet) == HIGH) { cambien = 1; }  // digitalread là hàm đọc giá trị  = high nghĩa là có cảm biến và gán cảm biến về 1
    else { cambien = 0;  } // ngược lại thì là 0 có 
}

void loop() // hàm thực hiện lệnh 
{
   unsigned long currentTime = millis();   // Hàm millis() đo thời gian kể từ khi Arduino được khởi chạy

  // Kiểm tra xem đã đủ thời gian trôi qua chưa
  if (currentTime - previousTime >= interval) // trong trường hợp khai báo biến ở trên thì thời gian đo = 1 giây
  {
      Read_SRHC04(); // nhận tín hiệu của cảm biến hc-sr04
      Read_SRHC505();// nhận tín hiệu của cảm biến hc-sr505
      if(tg == "tien") // tiến tự động có điều kiện
      { 
        if(khoangcach<=50) // thì xe sẽ dừng lại
        {
          idle();     // hàm dừng lại của xe     
          trangThaiHoatDong = 5; // số  5 thay đổi được 
        }
        else
        {
          turnUp();       // hàm tiến   
          trangThaiHoatDong = 1; 
        }
      }
      
      if(tg == "tiencb") // tiến cưỡng bức hay là tiến thủ công
      {
        turnUp();
        trangThaiHoatDong = 6;
      }
      
      if(tg == "lui") // hàm lùi
      {
        turnDown();        
        trangThaiHoatDong = 2;
      }
      
      if(tg == "trai") // hàm rẽ trái
      {   
        turnLeft();        
        trangThaiHoatDong = 3;
      }
      
      if(tg == "phai") // hàm rẽ phải
      
        turnRight();        
        trangThaiHoatDong = 4;
      }
      
      if(tg == "dung") // hàm dừng
      {
        idle();        
        trangThaiHoatDong = 5;
      }

      if(tg == "tangtoc") // hàm tăng tốc
      {
        trangThaiHoatDong = 7;
        speedUp();  // hàm tăng tốc      
      }
      
       // In ra giá trị về khoảng cách, trạng thái xe, cảm biến, tốc độ lên SerialMonitor
      Serial.print("Khoang cach: "); // in nội dung ra màn hình seriaMonitor
      Serial.print(khoangcach);
      Serial.println(" cm");

      Serial.print("Trang thai hoat dong: ");
      Serial.println(trangThaiHoatDong); // in ra giá trị trạng thái

      Serial.print("Cam bien: ");
      Serial.println(cambien);
    
      Serial.print("Toc do: ");
      Serial.println(i); // i tốc độ xe
    previousTime = currentTime; // Gán lại giá trị của biến previousTime = currentTime
  }
   // => Cơ chế sử dụng bộ đếm thời gian thay cho delay() giải quyết vấn đề tính không đồng bộ hoặc mất mát dữ liệu khi chạy chương trình
   // delay(1000);
}

// Hàm nhận dữ liệu từ ESP
/* Hàm này được tự động gọi khi có dữ liệu gửi đến Arduino (hàm này được khai báo trong void setup() )*/
void receiveEvent(int howMany) // tham số howMany là số lượng byte nhận ( rêciveEVENT là hàm kêu gọi sự kiện nhận dữ liệu của giao thức i2c giữa aduno và esp)
{
  String nhandulieu=""; // khai báo biến "nhanduieu" là 1 chuỗi kí tự 
  while (0 <Wire.available())  { // Vòng while chạy khi còn dữ liệu trong bộ đệm nhận của hàm (available là có sẵn)
    char c = Wire.read();        /* Biến c nhận 1 byte dữ liệu đọc được từ hàm Wire.read(), sau đó thêm byte đã đọc được vào biến nhandulieu */ 
    nhandulieu+=c; // nhận từ c++
  }   
  // Gán giá trị ngầm định cho biến tg thông qua dữ liệu mà biến nhandulieu nhận được
    if(nhandulieu == "1") // các số đều thay đổi được vì gán 
  {
    tg="tien";
  }
    if(nhandulieu == "2")
  {
    tg="lui";
  }
    if(nhandulieu == "3")
  {
    tg="trai";
  }
    if(nhandulieu == "4")
  {
    tg="phai";
  }
    if(nhandulieu == "5")
  {
    tg="dung";
  }
    if(nhandulieu == "6")
  {
    tg="tiencb";
  }
    if(nhandulieu == "7")
  {
    tg="tangtoc";
  }
}

void requestEvent() //Hàm gửi dữ liệu cho ESP 
{ 
  // Tạo một mảng chứa dữ liệu cần gửi
   byte dataToSend[4];  // Thay đổi kích thước mảng tùy theo số lượng dữ liệu bạn muốn gửi số [4] có thể thay đổi được do số lượng dữ liệu muốn gửi 

//   // Gán giá trị dữ liệu vào mảng
  dataToSend[0] = khoangcach;          // Gửi dữ liệu của khoangcach
  dataToSend[1] = cambien;             // Gửi dữ liệu cambien
  dataToSend[2] = trangThaiHoatDong;   // Gửi dữ liệu trangThaiHoatDong
  dataToSend[3] = i;                  // Gửi dữ liệu tốc độ   

   Wire.write(dataToSend, sizeof(dataToSend)); // Gửi mảng dữ liệu vừa xây dựng, hàm sizeof() cho biết kích thước mảng vừa gửi

  // delay(1000);   Đừng cho hàm delay vào, việc sử dụng delay() sẽ ngắt mọi thao tác của chương trình 
  //              => ảnh hưởng đến trao đổi dữ liệu I2C và thu nhận tín hiệu cảm biến
}
