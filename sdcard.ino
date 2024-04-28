#include <EEPROM.h>
#include "SD.h"
#include "SPI.h"
#include "FS.h"
//DI-23 DO-19 SCK-18 CS-5 VCC-3.3 GND-GND
// SD card pins
// Define the range of EEPROM memory to generate random data
#define START_ADDRESS 5
#define END_ADDRESS 20


// Khởi tạo biến để lưu trữ dữ liệu
const int dataSize = 5; // Số lượng dữ liệu
String data[dataSize];
/*void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }
    Serial.print("Read from file: ");
    while(file.available()){
        byte value = file.read();
        Serial.print(value);
        Serial.print(" ");
    }
    file.close();
}*/
/*void writeFile(fs::FS &fs, const char * path){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    for (int address = START_ADDRESS; address <= END_ADDRESS; address++) {
        byte value = EEPROM.read(address);
        file.write(value);
    }

    file.close();
    Serial.println("Data saved to SD card.");
}
*/
/*void generateRandomData() {
  randomSeed(analogRead(0));  // Seed the random number generator

  for (int address = START_ADDRESS; address <= END_ADDRESS; address++) {
    byte value = random(256);  // Generate a random byte (0 to 255)
    EEPROM.write(address, value);
    Serial.print(value); Serial.print(" ");
  }
  Serial.println("\n");
  Serial.println("Random data generated and saved to EEPROM.");
}
*/
/*void saveDataToSDCard() {
  // Open a file for writing
  Serial.println("Creating hello.txt...");
  File file = SD.open("/hello.txt", FILE_WRITE);

  //dataFile.close();

  if (file) {
    for (int address = START_ADDRESS; address <= END_ADDRESS; address++) {
        byte value = EEPROM.read(address);
        file.print(value);
        file.print(" ");
        
    }
  }
else{
  Serial.println("Error opening hello.txt on SD card for writing.");
}
    
    file.close();
    Serial.println("Data saved to SD card.");
}
*/
void writeDataToCSV() {
  // Mở file để ghi
  File file = SD.open("/data.csv", FILE_WRITE);
  if (!file) {
    Serial.println("Không thể mở file CSV để ghi.");
    return;
  }

  // Lấy thời gian hiện tại
  unsigned long currentTime = millis();

  // Ghi thời gian và dữ liệu vào file
  file.print(currentTime);
  for (int i = 0; i < dataSize; i++) {
    file.print(",");
    file.print(data[i]);
  }
  file.println(); // Xuống dòng để bắt đầu dòng mới
  file.close();

  Serial.println("Dữ liệu đã được ghi vào file CSV.");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
 Serial.println("Initializing SD card...");
  // Initialize EEPROM
 

  // Initialize SD card
   if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

  /*EEPROM.begin(512);
  // Generate random data and save it to EEPROM
  generateRandomData();

  // Save data from EEPROM to SD card
  saveDataToSDCard();
*/
File file = SD.open("/data.csv", FILE_WRITE);
  if (!file) {
    Serial.println("Không thể tạo file CSV.");
    return;
  }
  file.println("ThoiGian,Data1,Data2,Data3,Data4,Data5"); // Ghi header
  file.close();

  // Mô phỏng dữ liệu
  for (int i = 0; i < dataSize; i++) {
    data[i] = "Dulieu" + String(i + 1);
  }

  // Ghi dữ liệu vào file CSV
  writeDataToCSV();
}
  // Read data from SD card and print to Serial Monitor
  //readFile(SD, "/hello.txt");


void loop() {
  // Your main code here
  /*generateRandomData();
  delay(3000);
  saveDataToSDCard();
  delay(3000);
  readDataFromSDCard();
  delay(3000);*/
}
