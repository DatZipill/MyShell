# Hướng dẫn sử dụng MyShell (Updated 11/12/2025)
## Một số lệnh:
### 1.Mở tiến trình ở chế độ foreground:
Sử dụng lệnh "fopen". Khi mở ở chế độ này, shell sẽ tạm dừng đến khi tiến trình kết thúc.
#### Cú pháp:
```bash
fopen <tien_trinh_can_mo>
```
Ví dụ: fopen notepad
### 2.Mở tiến trình ở chế độ background:
Sử dụng lệnh "bopen". Khi mở ở chế độ này, shell sẽ hoạt động đồng thời với tiến trình.
#### Cú pháp:
```bash
bopen <tien_trinh_can_mo>
```
Ví dụ: bopen notepad

### 3. In ra danh sách tiến trình:
Sử dụng lệnh "list"
```bash
list
```

### 4. Hủy tiến trình:
Sử dụng lệnh "remove". Shell sẽ hiện danh sách tiến trình đang có và bạn sẽ chọn tiến trình bạn muốn theo số thứ tự của tiến trình đó.
```bash
remove
```

### 5. Tạm dừng và tiếp tục tiến trình:
Sử dụng lệnh "stop" và "resume". Shell sẽ hiện danh sách tiến trình đang có và bạn sẽ chọn tiến trình bạn muốn theo số thứ tự của tiến trình đó.
```bash
stop
```
```bash
resume
```

### 6. In ra một số lệnh hiện có:
Sử dụng lệnh "help". Shell sẽ in ra danh sách các lệnh của MyShell.
```bash
help
```

### 7. In ra thời gian thực:
Sử dụng lệnh "time". Shell sẽ in ra thời gian thực.
```bash
time
```

### 8. Path:
In ra các path đã được thêm vào shell
```bash
path
```
Thêm path vào danh sách các path của shell. 
```bash
addpath
```
### 9. Làm việc với thư mục hiện tại
In ra các file có trong thư mục hiện tại
```bash
dir
```
Di chuyển đến thư mục chỉ định
```bash 
cd <ten_thu_muc>
```

### 10. Tắt và thoát MyShell
```bash
exit
```

### 
## Tính năng:
-- Nhận tín hiệu ngắt từ bàn phím để hủy bỏ foreground process đang thực hiện (CTRL + C)

