create database xedieukhien
use xedieukhien

create table TrangThaiXe (
   id int not null primary key AUTO_INCREMENT,
   trangthaixe nvarchar(20),
   khoangcach int,
   cambien nvarchar(30),
   tocdoxe int
)

SELECT * FROM trangthaixe
