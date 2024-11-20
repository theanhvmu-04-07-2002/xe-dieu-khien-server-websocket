// Các module được sử dụng bao gồm:
// fs (để làm việc với tệp tin), url (để xử lý URL), 
// http (tạo máy chủ HTTP), WebSocket (cung cấp dịch vụ WebSocket), 
// và mysql (để kết nối và thao tác với cơ sở dữ liệu MySQL).
var fs = require('fs'); // Import các module cần để xây dựng server
var url = require('url');
var http = require('http');
var WebSocket = require('ws');
var mysql = require('mysql');

var temp = 0;
var tempError = 0;
// function gửi yêu cầu(response) từ phía server hoặc nhận yêu cầu (request) của client gửi lên
function requestHandler(request, response) {  // Hàm được gọi khi có  yêu cầu http đến server
    fs.readFile('./BTL.html', function(error, content) { // Đọc nội dung của file BTL.html
        response.writeHead(200, {
            'Content-Type': 'text/html'
        });
        response.end(content); // gửi nội dung đã đọc từ file BTL.html về client, sau đó đóng kết nối
    });
}

// create http server
var server = http.createServer(requestHandler); //tạo máy chủ http
var ws = new WebSocket.Server({ //tạo máy chủ websocket liên kết với máy chủ http
    server
});

// Hàm broadcast nhằm mục đích gửi dữ liệu đến tất cả các các kết nối socket khác ngoài socket gốc
var clients = [];

function broadcast(socket, data) {
    console.log(clients.length);
    for (var i = 0; i < clients.length; i++) {
        if (clients[i] != socket) {
            clients[i].send(data);
        }
    }
}

// Kết nối đến MySQL với các thông tin tương ứng
var con = mysql.createConnection({
    host:"localhost",  
    user: "admin",  // tên của người dùng admin sql trên phần mềm mysqlbenmark
    password: "admin123",
    database: "xedieukhien", // tên dữ liệu của bảng sẽ lưu vào trong mysql
});

   con.connect(function(err){
       if(err) throw err;

       console.log("MySQL connected");

        var sql ="SELECT * FROM TrangThaiXe";

        con.query(sql,function(err,result){ // Chạy cầu lệnh sql khi kết nối thành công
            if(err)throw err;
        });

        ws.on('connection', function(socket, req) { // Hàm được gọi mỗi khi có 1 kết nối socket mới

            console.log('Client connected');
            clients.push(socket); // Thêm đối tượng WebSocket hiện tại vào mảng clients, để theo dõi tất cả các kết nối đang hoạt động.

            socket.on('message', function(message)  // Hàm xử lí dữ liệu khi có kết nối socket
            {
                var data = JSON.parse(message); // Nhận dữ liêu thông qua biến message, biến data sẽ parse dữ liệu từ JSON thành đối tượng mảng
                console.log('received khoangcach:', data.khoangcach); // Truy cập vào giá trị của khoangcach
                console.log('received cambien:', data.cambien); // Truy cập vào giá trị của cambien
                console.log('received trangthai:', data.trangthai); // Truy cập vào giá trị của trangthai
                console.log('received tocdo:', data.tocdo); // Truy cập vào giá trị của tocdo
                console.log('received: %s', message);
                broadcast(socket, message);  // Gửi dữ liệu nhận đc đến các kết nối khác
                if(data.khoangcach <= 50 && data.trangthai == 5) // điều kiện để thêm dữ liệu vào bảng csdl là khi khoảng cách nhỏ hơn hoặc bằng 50 và trạng thái = 5 tức là dừng lại.
                {
                    // thực hiện thêm vào bảng csdl
                    con.query("INSERT INTO TrangThaiXe (trangthaixe, cambien, tocdoxe, khoangcach) VALUES('dừng', '"+data.cambien+"', '"+data.tocdo+"', '"+data.khoangcach+"')",function(err,result){
                    if(err)throw err;
                     console.log('insert data successfully');                                                
                    });                                    
                }
                
            });
            socket.on('close', function() {  // Hàm gọi khi 1 kết nối đóng, đối tượng websocket tương ứng bị loại khỏi mảng clients[]
                var index = clients.indexOf(socket);
                clients.splice(index, 1);
                console.log('disconnected');
            });           
        });
    });

server.listen(3000); // Server lắng nghe các phản hồi thông qua cổng = 3000
console.log('Server listening on port 3000');