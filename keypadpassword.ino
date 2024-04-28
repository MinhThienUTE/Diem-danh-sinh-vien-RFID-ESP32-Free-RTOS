#include "Keypad.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {19, 18, 5, 17}; 
byte colPins[COLS] = {16,4,0,2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int len_key = 5;
char master_key[len_key] = {'1','2','3','4','5'};
char attempt_key[len_key];
int z = 0;

void clearInput() {
  for (int i = 0; i < len_key; i++) {
    attempt_key[i] = 0;
  }
  z = 0;
  lcd.setCursor(0, 1);
  lcd.print("    "); // Clear the input display on LCD
}

void setup() {
  Serial.begin(9600);
  lcd.init();      // khoi tao man hinh  
  lcd.backlight(); // bat den man hinh
  lcd.setCursor(0, 0); // chon vi tri dat con tro
  lcd.print("Nhap ID:");
}

void loop() {
  char key = keypad.getKey();
  if (key && key != '*' && key != '#') {
    if(z<=4){
    lcd.setCursor(z, 1);
    lcd.print("*");
    }
  }
  if (key) {
    switch (key) {
      case '*':
        clearInput();
        break;
      case '#':
        delay(100); // added debounce
        checkKEY();
        break;
      case 'A':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Moi them ID:");
        break;
      case 'C':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Xoa tat ca ID:");
        break;
      case 'D':
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Xoa ID:");       
        break;  
      default:
        if (z < len_key) {
          attempt_key[z] = key;
          z++;
        }
        if (z == len_key) {
          break; // Ngăn chặn thêm kí tự khi z đã đạt đến len_key
        }
    }
  }
}

void checkKEY() {
  int correct = 0;
  int i;
  for (i = 0; i < len_key; i++) {
    if (attempt_key[i] == master_key[i]) {
      correct++;
    }
  }
  if (correct == len_key && z == len_key) {
    lcd.setCursor(0, 1);
    lcd.print("ID hop le");
    delay(2000);
    clearInput();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nhap ID:");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("ID khong ton tai");
    delay(2000);
    clearInput();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nhap ID:");
    
  }
}
