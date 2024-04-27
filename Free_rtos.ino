                            //HỆ ĐIỀU HÀNH THỜI GIAN THỰC//
                        // PROJECT: Hệ thống điểm danh sinh viên///
////////////////////////////////Sơ đồ nối chân//////////////////////////////////////////////////////
//    ESP32        MFRC522            KEYPAD            LCD_I2C         DS1307          SD_CARD   //
//    5             SDA                                                                               
//    18            SCK
//    23            MOSI
//    19            MISO
//    gnd           GND                                   GND           GND
//    3V3           3V3                                                 VCC
//    27            RST                         
//    21                                                  SDA           SDA
//    22                                                  SCL           SCL
//    5V                                                  VCC
//    4                                 1(từ trái)
//    33                                2
//    25                                3
//    16                                4
//    17                                5
//    0                                 6
//    32                                7
//    26                                8
//    12                                                                                   MISO
//    13                                                                                   MOSI
//    14                                                                                   SCK
//    15                                                                                   CS
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <Arduino.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "FS.h"
#include "RTClib.h"
SemaphoreHandle_t sem1;//
SemaphoreHandle_t sem2;
TaskHandle_t task1;  //bien 
TaskHandle_t task2;
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
  //Timer handle, co the dung cho cac lenh vTaskDelete, vTaskSuspend,...
RTC_DS1307 rtc;                                                                                                //Khởi tạo một đối tượng của thư viện RTClib là rtc
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
byte rowPins[ROWS] = { 4, 33, 25, 16 };
byte colPins[COLS] = { 17, 0, 32, 26 }; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
const int len_key = 4; // Độ dài của mật khẩu
char master_key_A[len_key] = {'0', '0', '0', '1'}; // Mật khẩu cho hàm A
char master_key_B[len_key] = {'0', '0', '0', '2'}; // Mật khẩu cho hàm B
char master_key_C[len_key] = {'0', '0', '0', '3'}; // Mật khẩu cho hàm C
char master_key_D[len_key] = {'0', '0', '0', '4'};
char attempt_key[len_key];
int z = 0;
int z1 = 0;
const int len_key_mssv = 8;
char attempt_key1[len_key_mssv];
LiquidCrystal_I2C lcd(0x27,16,2);
#define RST_PIN         27
#define SS_PIN          5
#define FLASH_MEMOMRY_SIZE 200
#define SD_CS_PIN 15
#define SD_MOSI 13
#define SD_MISO 12
#define SD_SCK 14
SPIClass SPISD(HSPI);
unsigned long UID[4];
unsigned long i;
int diachi = 1; int diachi2 = 0; int a;
int gtmas = 0; int tong = 0; int gttong = 0;
int bandau;
int macdinh = 1;
int dem_tong = 0; int dem_menu = 0; int dem = 0;
MFRC522 mfrc522(SS_PIN, RST_PIN);
File myFile;
int id_moi[4]; int id_er[4];
    int diachi_zero; 
    int m = 5; int moi = 0;
    int gt_zero;
    int gt_er = 0; 
    int n = 4; int o=0;
  int demco = 0; int demchua = 0; int demmas = 0;
String uidString;
int userCheckInHour;
int userCheckInMinute;
// xóa hiển thị LCD//
void clearInput() {
  for (int i = 0; i < len_key; i++) {
    attempt_key[i] = 0;
  }
  z = 0;
  lcd.setCursor(0, 1);
  lcd.print("        "); // Xóa hiển thị đầu vào trên LCD
}
// tìm ô trống đầu tiên trong eeprom//
void tim_zero(){
  while( n < a) //Tìm ô nhớ có GT 0 đầu tiên
    {
      gt_zero = EEPROM.read(n);
      if (gt_zero == 0)
      {
        diachi_zero = n;
        break;
      }      
      n++;
    }
  //Serial.println(".....................");
  if(diachi_zero==0)
  {
    diachi_zero = a; //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                    // thì diachi_zero = a là ô cuối cùng lưu thẻ
    n=0;
  } 
}
// lưu các thẻ vào eeprom//
void ss_epprom(){
  //Serial.print("Zero: "); Serial.println(diachi_zero);
  //m = 5 ; a lưu số ô cuối được dùng
    while(m < a) //chạy từ ô 5 đến ô sử dụng cuối ... 5 < 20
    {
      moi = m + 4; //moi = 9
      for(m; m<moi; m++)
      {
        gt_er = EEPROM.read(m);
        // Serial.print("m: "); Serial.print(m);
        // Serial.print("   moi: "); Serial.print(moi);
        // Serial.print("   GT eep: "); Serial.print(gt_er);
        // o=0
        if(o<=3) //Lưu giá trị đọc từ eeprom vào id_er
        {
          id_er[o]=gt_er;
          // Serial.print("   id eeprom: "); Serial.print(id_er[o]);
          // Serial.print("   o: "); Serial.println(o);
        }
        o++;
      }   
      // Serial.print("$$$$$$$$$$ o: "); Serial.println(o);
      if(id_moi[0] == id_er[0] && id_moi[1] == id_er[1] && id_moi[2] == id_er[2] && id_moi[3] == id_er[3]) //Nếu thẻ có trong EEPR
      {
        demco = 1; demchua=0; demmas=0;
        //Serial.print("   demco: "); Serial.println(demco);
        break;
      }
      else if(id_moi[0] != id_er[0] or id_moi[1] != id_er[1] or id_moi[2] != id_er[2] or id_moi[3] != id_er[3]) //Nếu thẻ KO có trong EEPR
      { // Dùng or vì chỉ cần  1 byte khác với 1 byte lưu trong eeprom thì thẻ đó là thẻ khác
        // Nếu dùng && có thể có thẻ sẽ trùng 1 đến 2 byte
        demchua += 1; demco=0; demmas=0;
        //Serial.print("   demchua: "); Serial.println(demchua);
      } 
      if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
              id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER
      {
        demmas=1; demco=0; demchua=0;
      }           
      o = 0;
      m = moi;
    }  
}
// chọn mode//
void menu_tong(){
  xSemaphoreGive(sem1);
 // lcd.clear();
   lcd.setCursor(0,0);
  lcd.print("     MODE    ");
  //nhapkeypad();
  //xSemaphoreGive(sem1);
}
//Đọc giá trị từ keypad//
void nhapkeypad(void*){
  vTaskSuspend(task1);
    //lcd.print("nhap opcode");
  xSemaphoreTake(sem1, portMAX_DELAY);
  while(1){
    Serial.println("keyp");
    lcd.setCursor(0,1);
    lcd.setCursor(z, 1);
    char key = keypad.getKey();
    if (key) {
      switch (key) {
        case '*':
          clearInput();
          break;
        case '#':
          //delay(100); // Chống nhấp nháy     
          vTaskDelay(pdMS_TO_TICKS(100));
          checkKey();
          break;
        default:
          if (z < len_key) {
            attempt_key[z] = key;
            lcd.print('*');
            z++;
          }
          if (z == len_key) {
            break; // Ngăn chặn thêm ký tự khi z đạt đến len_key
          }
      }
    } 
    
   vTaskDelay( pdMS_TO_TICKS(100));}
  //xSemaphoreGive(Sem_Handle);
}
////////////////////////////////
// 0001: Thêm sinh viên       //
// 0002: Xóa sinh viên bất kỳ //
// 0003: Xóa tất cả sinh viên //
// 0004: Điểm danh sinh viên  //
////////////////////////////////
void checkKey() {
    int correct_A = 1; // Biến kiểm tra mật khẩu cho hàm A
    int correct_B = 1; // Biến kiểm tra mật khẩu cho hàm B
    int correct_C = 1; // Biến kiểm tra mật khẩu cho hàm C
    int correct_D = 1;
    // Kiểm tra mật khẩu cho hàm A
    for (int i = 0; i < len_key; i++) {
      if (attempt_key[i] != master_key_A[i]) {
        correct_A = 0;
        break; // Nếu có ký tự nào không khớp thì thoát khỏi vòng lặp
      }
    }

    // Kiểm tra mật khẩu cho hàm B
    for (int i = 0; i < len_key; i++) {
      if (attempt_key[i] != master_key_B[i]) {
        correct_B = 0;
        break; // Nếu có ký tự nào không khớp thì thoát khỏi vòng lặp
      }
    }

    // Kiểm tra mật khẩu cho hàm C
    for (int i = 0; i < len_key; i++) {
      if (attempt_key[i] != master_key_C[i]) {
        correct_C = 0;
        break; // Nếu có ký tự nào không khớp thì thoát khỏi vòng lặp
      }
    }

  for (int i = 0; i < len_key; i++) {
      if (attempt_key[i] != master_key_D[i]) {
        correct_D = 0;
        break; // Nếu có ký tự nào không khớp thì thoát khỏi vòng lặp
      }
    }

    // Xử lý tương ứng với mật khẩu đúng hoặc sai
    if (correct_A) {
      lcd.clear();
      lcd.setCursor(0, 0);
           
      lcd.print("Them the moi");
       themthemoi();
       // Gọi hàm A
    } else if (correct_B) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Xoa mot the");
      dem_tong = 2;
      xoathebatky();
    } else if (correct_C) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Xoa tat ca the");
      dem_tong == 3;
      xoatatcathe();
    }  else if (correct_D) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Diem danh");
      //menu_tong();
      diemdanh(); // Gọi hàm C
      //delay(1000);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sai yeu cau");
      delay(2000);
      //vTaskDelay( pdMS_TO_TICKS(100));
      lcd.clear();
      lcd.print("Moi nhap lai");
      delay(1000);
      //vTaskDelay( pdMS_TO_TICKS(100));
      clearInput();
      menu_tong();
    }

    delay(3000); // Hiển thị kết quả trong 1 giây
    clearInput();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nhap yeu cau");
    lcd.clear();
    //diemdanh();
}
// Điểm danh và lưu vào sdcard//
void diemdanh() {
  Serial.println("diemdanh");
  while(1){
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }

  for (byte i = 0; i < 4; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " " : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    UID[i] = mfrc522.uid.uidByte[i];
    id_moi[i] = UID[i];
  }
  uidString = String(id_moi[0]) + " " + String(id_moi[1]) + " " + 
    String(id_moi[2]) + " " + String(id_moi[3]);
  Serial.println(uidString);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  ss_epprom(); // So sánh thẻ mới với EEPROM

  if (demco == 1 && demchua == 0 && demmas == 0) {
    o = 0;
    m = 5;
    moi = 0;
    demco = 0;
    demchua = 0;
    demmas = 0;
    lcd.setCursor(0, 1);
    //Serial.print(" ds1307, sd card, tre/dungio,mssv");
    Serial.print("   DONE   ");
    lcd.print("DONE");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Diem danh");
    delay(500);
    ds1307();
    load_data();
    //xSemaphoreGive(sem3);
    delay(1000);
  } else if (demchua > 0 && demco == 0 && demmas == 0) {
    o = 0;
    m = 5;
    moi = 0;
    demco = 0;
    demchua = 0;
    demmas = 0;
    lcd.setCursor(0, 1);
    lcd.print("    SAI THE!    ");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Diem danh");
    //delay(500);
    delay(2000);
    //lcd.clear();
  } else if (demmas == 1 && demco == 0 && demchua == 0) {
    o = 0;
    m = 5;
    moi = 0;
    demco = 0;
    demchua = 0;
    demmas = 0;
    lcd.clear();
    menu_tong();
    dem_menu = 0;
    //xSemaphoreGive(sem3);
    break;
  }
  }

  //xSemaphoreGive(sem3);
}
//điều khiển hệ thống  bằng 1 thẻ MASTER//
void checkMas(void*){
  xSemaphoreGive(sem2);
  while(1){
    xSemaphoreTake(sem2, portMAX_DELAY);
    bandau=true;
    if(bandau == true) //Kiểm tra có thẻ MAS chưa
      {
      for(int j = 1; j < 4; j++) // đọc từ 1 - 4 xem trong ô nhớ có gì ko, Nếu không là chưa có thẻ MAS
      {
          gtmas = EEPROM.read(j);     //doc EEPROM
          gttong = gttong + gtmas;    //
            //Serial.print("Gtmas: "); Serial.print(gtmas); Serial.print("  ");
           // Serial.print("Tổng: "); Serial.print(tong); Serial.print("  ");
           Serial.print(" mas");
      } 
      }
            
    if (gttong == 0) //Chưa có thẻ MAS
     {
      tong = 0;
      if (tong == 0 && dem == 0)//Nếu ô 1, 2, 3, 4 chưa có Mas thì nhập Mas
      { 
        lcd.setCursor(2,0);
        lcd.print("SETUP MASTER");
        lcd.setCursor(0,1);
        lcd.print("MOI NHAP THE...");
       // vTaskDelay(pdMS_TO_TICKS(3000));
        if ( mfrc522.PICC_IsNewCardPresent()) 

        if ( mfrc522.PICC_ReadCardSerial()) 

        for (byte i = 0; i < 4; i++) 
        { 
              Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " " :" "); 
              Serial.print(mfrc522.uid.uidByte[i], HEX);         
          UID[i] = mfrc522.uid.uidByte[i];

          EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 1-4
          diachi = diachi + 1;
        }
        mfrc522.PICC_HaltA();  
        mfrc522.PCD_StopCrypto1();
        //delay(1000);
        xSemaphoreGive(sem2);
        //vTaskDelay( pdMS_TO_TICKS(1000));
       // lcd.clear();  
      }
    } 
    else // Đã có thẻ MAS
      {
        tong = 1; bandau = false;
        if (tong == 1 && dem == 0) // Đã có MASTER
        { 
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("Scan!");
          lcd.setCursor(0,1);
          lcd.print("SCAN MAS again...");
          //xSemaphoreGive(sem1);
          //delay(1000);
          //vTaskDelay( pdMS_TO_TICKS(100));
          vTaskDelay(pdMS_TO_TICKS(3000));
          if (  mfrc522.PICC_IsNewCardPresent())     //doc the mot lan duy nhat
          {         
            if (  mfrc522.PICC_ReadCardSerial())      //doc toan bo du lieu trong thẻ
              {
                  for (byte i = 0; i < 4; i++) 
                    {        
                      UID[i] = mfrc522.uid.uidByte[i];    //id cua the 
                      id_moi[i] = UID[i];                 //id moi cua the = id the
                    }
                  mfrc522.PICC_HaltA();    //tam ngung doc
                  mfrc522.PCD_StopCrypto1(); //ngung viec su dung rfid
                  lcd.clear();
                  xSemaphoreGive(sem1);
                  if (id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2)        
                      && id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //Check xem phải thẻ MAS ko
                  {
                    dem = 1;
                    if (tong == 1 && dem == 1) //Để thẻ MASTER để vào vào menu
                    {
                    // xSemaphoreGive(sem1);
                        lcd.clear();
                        menu_tong();    //nhay toi menutong
                    }   
                  }
                  else{ 
                    lcd.clear();            //xoa man hinh
                    lcd.setCursor(0, 0);
                    lcd.print("Sai yeu cau"); 
                    delay(1000);       
                    //vTaskDelay( pdMS_TO_TICKS(100));
                    //vTaskDelay( pdMS_TO_TICKS(100));
                    lcd.clear();
                    lcd.setCursor(0, 1);
                    lcd.print("Moi nhap lai");
                    delay(1000);
                    //vTaskDelay( pdMS_TO_TICKS(100));
                    lcd.clear();
                    xSemaphoreGive(sem2);
                    }
                }
            }

        }  
      }
            // vTaskDelay( pdMS_TO_TICKS(1000));
      //xSemaphoreGive(Sem_Handle);
  }           

}
//Thêm sinh viên mới//
void themthemoi(){
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  Serial.print(" a: "); Serial.println(a); 
  
  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  { 
    int diachi_phu = 5;
    Serial.println("chưa có thẻ phụ");

    while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    { 
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " " :" "); 
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
    
    
    if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
        id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER  
    {
      lcd.clear();
      menu_tong();
      dem_menu = 0;      
    }
    
    else //THẺ MỚI KHÔNG PHẢI LÀ THẺ MASTER thi LƯU vào ô 5,6,7,8
    { lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(" Dang luu the "); 
    delay(2000);
    lcd.clear();

      for (byte i = 0; i < 4; i++)
      {
        EEPROM.write(diachi_phu, id_moi[i]);
        diachi_phu = diachi_phu + 1;
        a = diachi_phu;
      }
      EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      lcd.setCursor(0,1);      
      menu_tong();       
      
  }
  }
  else if( a != 0) // Đã có 1 or nhiều thẻ phụ
  {
    while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    Serial.println(id_moi[i]);
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  

    ss_epprom(); //So sánh thẻ mới đưa vào với Eeprom
    
    if(demco == 1 && demchua == 0 && demmas == 0) //Nếu thẻ đã có
    {
        // Serial.print("................THE DA CO TRONG EEPROM..................");
        // Serial.print(" Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco);           
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print("   THE DA CO!   "); 
      delay(1000); 
      lcd.clear();
      
      //chon_menu(); 
      //in();
    }
    else if(demchua > 0 && demco == 0 && demmas == 0) //Nếu thẻ chưa có...THÌ LƯU THẺ ĐÓ VÀO EEPROM
    {
      tim_zero(); //Tìm vị trí 0 đầu tiên
      if(diachi_zero == a) //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                           // a là ô cuối cùng lưu thẻ       
      {
        for(int i=0; i<4; i++)
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          a = diachi_zero;
        }
        EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      }

      else if(diachi_zero == n) ////Nếu trong đoạn từ 4 đến số ô đã sử dụng CÓ ô = 0, thì gán ô đó vào n
      {
        for(int i=0; i<4; i++) //Lưu thẻ mới vào bắt đầu từ ô 0 đó
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          //a = diachi_zero;
        }        
        diachi_zero=a;
      }
      tim_zero();
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print(" Dang luu the "); 
      delay(1000); 
      lcd.clear();
      lcd.setCursor(0,1);
    }
     if(demmas == 1 && demco == 0 && demchua == 0) //THẺ MASTER thì quay về menu
    {
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0; 
      //in();            
    }
  }
}
//Xóa 1 sinh viên bất kỳ//
void xoathebatky(){
  //if (dem_menu == 1 && dem_tong == 2 && dem == 1) //Nếu chọn XÓA THẺ BẤT KỲ

  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 

  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" CHUA CO THE... ");
    delay(1000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;     
  }
  else //Nếu đã có 1 hoặc nhiều thẻ PHỤ
  {
    while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  
    ss_epprom();
    if(demco == 1 && demchua == 0 && demmas == 0) //thẻ có trong EEPROM
    {
      tim_zero();
      m = m - 4;
      for(int i=0; i<4; i++)
      {
        EEPROM.write(m, 0);
        m = m + 1; 
      }
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; n=4;
      lcd.setCursor(0,1);
      lcd.print(" DA XOA THE.... "); 
      delay(1000); 
      lcd.clear();
      //chon_menu();
      lcd.setCursor(0,0);
      //lcd.print("nhap mssv");
      tim_zero();
      //in();      
    }
    else if (demchua > 0 && demco == 0 && demmas == 0) //Thẻ chưa có trong EEPROM
    {         
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; 
      lcd.setCursor(0,1);
      lcd.print(" THE CHUA CO... "); 
      delay(1000); 
      lcd.clear();
      //chon_menu();  
      //in();      
    }
    if(demmas == 1 && demco == 0 && demchua == 0) //Là thẻ MASTER
    {
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0;
    }
  }
}
//Xóa tất cả sinh viên//
void xoatatcathe(){
  /*a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 
  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" CHUA CO THE... ");
    dem_menu = 0;
    delay(2000);
    lcd.clear();
    menu_tong();
  } */
  //else if (a > 0) //Nếu đã có 1 hoặc nhiều thẻ PHỤ
    lcd.setCursor(0,1);
    lcd.print("  DANG XOA....  "); 
    for (int diachi = 5; diachi<200; diachi++) //Bắt đầu từ ô 5 đến ô đang sử dụng
    {
      EEPROM.write(diachi, 0);
    }  
    EEPROM.write(0, 0); //Sau đó cập nhật vào ô 0 là 0 vì đã xóa hết thẻ   
    delay(3000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;
}
// cài đặt ds1307//
void setup_DS1307() {
  if (!rtc.begin())  //Kiểm tra xem module RTC có được kết nối hay không
  {
    Serial.print("Couldn't find RTC");  //Nếu không thì in ra thông báo "Couldn't find RTC" và dừng chương trình
    Serial.flush();
    while (1) vTaskDelay(10);
  }
  if (!rtc.isrunning())  //Kiểm tra xem module RTC có đang hoạt động hay không (đọc các thanh ghi bên trong I2C của DS1307 để kiểm tra xem chip có trả về thời gian hay không)
  {
    //Serial.print("RTC is NOT running!");  //Nếu không hoạt động, in ra thông báo "RTC is NOT running!" và tiếp tục chương trình
    Serial.println();
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //Cập nhật thời gian tự động từ máy tính
                                                   //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}
// kiểm tra thời gian vào,trễ, đúng giờ//
void ds1307(){
  setup_DS1307();
  while(1){
    Serial.println("ds1307");
      DateTime now = rtc.now();   //Lấy thời gian hiện tại từ module RTC và lưu vào biến now
      int hour = now.hour();  //Lấy giờ hiện tại
      int minute = now.minute();  //Lấy phút hiện tại
      int second = now.second();  //Lấy giây hiện tại
      Serial.print(now.year());   //In ra năm hiện tại từ đối tượng now của lớp DateTime
      Serial.print('/');          //In ra màn hình "/"
      Serial.print(now.month());  //In ra tháng hiện tại từ đối tượng now của lớp DateTime
      Serial.print('/');
      Serial.print(now.day());
      Serial.print(' ');
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(" ");
      Serial.print(now.hour());
      Serial.print(':');
      Serial.print(now.minute());
      Serial.print(':');
      Serial.print(now.second());
      Serial.println();
      if (hour < 15 || (hour == 15 && minute <= 30)) {
      Serial.println("Đi đúng giờ!");
      lcd.setCursor(5,1);
      lcd.print("DUNG GIO");
      delay(1000);
      break;
      //lcd.clear();
      // Xử lý khi đi đúng giờ ở đây
    } else {
      Serial.println("Trễ giờ!");
      lcd.setCursor(5,1);
      lcd.print("TRE GIO");
      delay(1000);
      break;
      //lcd.clear();
      // Xử lý khi trễ giờ ở đây
    }
    //vTaskDelay(pdMS_TO_TICKS(1000));
    delay(500);
    }
      //lcd.clear();
      //xSemaphoreGive(sem4);
}
void setup_sdcard(){
  while (!Serial);
  SPISD.begin(SD_SCK, SD_MISO, SD_MOSI,SD_CS_PIN);
  if(!SD.begin(SD_CS_PIN,SPISD)){
        Serial.println("Card Mount Failed");
        return;
    }		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  Serial.println("Creating hello.txt...");
  File file = SD.open("/DATA.txt", FILE_WRITE);
  if(file){
    file.print("DATA");
  }
  else{
  Serial.println("Error opening hello.txt on SD card for writing.");
  }
  //file.close();
  Serial.println("Data saved to SD card.");

}
// lưu dữ liệu vào sdcard// 
void load_data(){
  //pinMode(SD_CS_PIN,OUTPUT);
  digitalWrite(SD_CS_PIN,LOW);
  // Open file
  myFile=SD.open("DATA.txt", FILE_WRITE);
  // If the file opened ok, write to it
  if (myFile) {
    Serial.println("File opened ok");
    myFile.print(uidString);
    myFile.print(", ");   
    
    // Save time on SD card
    DateTime now = rtc.now();
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.println(now.minute(), DEC);
    
    // Print time on Serial monitor
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.println(now.minute(), DEC);
    Serial.println("sucessfully written on SD card");
    myFile.close();

    // Save check in time;
    userCheckInHour = now.hour();
    userCheckInMinute = now.minute();
  }
  else {
    Serial.println("error opening data.txt");  
  }
  // Disables SD card chip select pin  
  digitalWrite(SD_CS_PIN,HIGH);
}
void setup() {
  Serial.begin(9600);  
  pinMode(SD_CS_PIN,OUTPUT);
  setup_sdcard();
  //SPISD.begin(SD_SCK, SD_MISO, SD_MOSI,SD_CS_PIN);
  SPI.begin();    
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  EEPROM.begin(FLASH_MEMOMRY_SIZE);
  for(int i=0; i<EEPROM.length(); i++)
 {
  EEPROM.write(i,0);
 }
  sem1 = xSemaphoreCreateBinary(); 
  sem2 = xSemaphoreCreateBinary();
  xTaskCreate(checkMas, "Task1", 4000, NULL, 2, &task1); // checkMas: nhập Mas, ss EPROM, tim_zerp, them/xoa thẻ,diemdanh,lưu sdcard
  xTaskCreate(nhapkeypad, "Task2", 2000, NULL, 3, &task2);// gồm: chọn chế độ, hiển thị lcd
  vTaskStartScheduler();
}
void loop() {}