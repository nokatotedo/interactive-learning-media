#include <Keypad.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int buttonStart = A4;
const int buttonRead = A5;

const int motor1 = 8;
const int motor2 = 9;

const int s0 = 4;
const int s1 = 3;
const int s2 = 6;
const int s3 = 7;
const int sOut = 5;
int redC, blueC, greenC, shape;
String color;
bool process = false;
int count;

const int ledG = A1;
const int ledY = A2;
const int ledR = A3;

const int buzzer = A0;

const byte rows = 4;
const byte cols = 3;
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'.','0','#'}
};
byte pinRows[rows] = {A8,A9,A10,A11};
byte pinCols[cols] = {A12,A13,A14};
Keypad kypd = Keypad(makeKeymap(keys),pinRows,pinCols,rows,cols);
String answer;
bool answering;
int answerTry;

SoftwareSerial bluetooth(51, 53);
char key;

SoftwareSerial serialMP3(11, 10);
int question;
int delayQuestion[21] = {
  0,
  0, 14000,20000,18000,21000,
  0, 21000,9000,12000,15000,
  0, 18000,20000,19000,19000,
  0, 12000,11000,10000,13000
};

String keyQuestion[21] = {
  "0",
  "0", "180", "5", "7", "36",
  "0", "10", "100", "13", "64",
  "0", "5", "460", "1250", "750",
  "0", "188.4", "153.86", "10", "20"
};

LiquidCrystal_I2C lcd(0x27, 16, 2);
int lcdCursor = 0;
int note;

void setup() {
  Serial.begin(9600);
  
  pinMode(buttonStart, INPUT);
  pinMode(buttonRead, INPUT);

  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sOut, INPUT);

  pinMode(ledR, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledG, OUTPUT);

  pinMode(buzzer, OUTPUT);

  bluetooth.begin(9600);
  
  serialMP3.begin(9600);
  mp3_set_serial(serialMP3);
  delay(10);
  mp3_set_volume(30);
  delay(10);
  
  lcd.begin();
  lcd.backlight();
  intro("Dwiky Pandu Aqri","19101152620057");
  intro("Iqsyan Yulfi","19101152620066");
  lcd.clear();
  lcd12("Tombol biru","untuk memulai");
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
}

void loop() {
  int statusStart = digitalRead(buttonStart);
  if(statusStart == LOW) {
    sensorOff();
    
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);

    lcd12("Memulai proses","pencarian... ");
    
    count = 1;
    process = true;
  } else {
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);

    sensorOn();
    if(count <= 3) count++;
    if((count == 3)&&(process)) {
      checkColor();
      lcd.clear();
      lcd12("Bangun datar",color);

      answering = true;
      if(shape == 1) {
        mp3_play(1); //rumus keliling dan luas
        delay(12000);
        questionSound(2,5);
        lcd.clear();
        lcd12("Jawaban kamu","");
      } else if(shape == 2) {
        mp3_play(6);
        delay(10000);
        questionSound(7,10);
        lcd.clear();
        lcd12("Jawaban kamu","");
      } else if(shape == 3) {
        mp3_play(11);
        delay(13000);
        questionSound(12,15);
        lcd.clear();
        lcd12("Jawaban kamu","");
      } else if(shape == 4) {
        mp3_play(16);
        delay(12000);
        questionSound(17,20);
        lcd.clear();
        lcd12("Jawaban kamu","");
      } else {
        answering = false;
      }
      
      answerTry = 0;
      while(answering) {
        bluetooth.listen();
        if(bluetooth.available()) {
          key = bluetooth.read();
        }
        if(key == 'U') {
          bluetooth.print("Jawabannya : ");
          bluetooth.println(keyQuestion[question]);
          mp3_play(question);
          delay(delayQuestion[question]);
        }
        answering = true;
        Serial.println(answering);
        answerQuestion();
      }

      if(note == 1) {
        mp3_play(25);
        lcd.clear();
        lcd12("Kamu hebat!","Selamat ^_^");
        delay(4000);
      } else {
        mp3_play(21);
        lcd.clear();
        lcd12("Ayo coba sekali","lagi!");
        delay(4000);
      }
      
      lcd.clear();
      lcd12("Tombol biru","untuk memulai");
    }
  }
}

void intro(String nameMhs, String noMhs) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(nameMhs);
  lcd.setCursor(0,1);
  lcd.print(noMhs);
  delay(3000);
}

void lcd12(String baris1, String baris2) {
  lcd.setCursor(0,0);
  lcd.print(baris1);
  lcd.setCursor(0,1);
  lcd.print(baris2);
}

void sensorOff() {
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
}

void sensorOn() {
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(s2,LOW);
  digitalWrite(s3,LOW);
  int redF = pulseIn(sOut, LOW);
  redC = map(redF, 25, 60, 255,0);
      
  digitalWrite(s3,HIGH);
  int blueF = pulseIn(sOut, LOW);
  blueC = map(blueF, 20, 80, 255, 0);
      
  digitalWrite(s2,HIGH);
  int greenF = pulseIn(sOut, LOW);
  greenC = map(greenF, 15, 95, 255, 0);
}

void checkColor() {
  if((redC < 10)&&(greenC < 10)&&(blueC < 10)) {
    shape = 0;
    color = "tidak ada";
  } else {
    if((blueC < redC)&&(blueC < greenC)) {
      shape = 1;
      color = "segitiga";
    } else if((redC > greenC)&&(redC > blueC)) {
      shape = 2;
      color = "persegi";
    } else if((greenC > redC)&&(greenC > blueC)) {
      shape = 3;
      color = "persegi panjang";
    } else if((blueC > redC)&&(blueC > greenC)) {
      shape = 4;
      color = "lingkaran";
    }
  }
}

void questionSound(int minFile, int maxFile) {
  Serial.println();
  mp3_play(27);
  delay(2000);
  question = random(minFile,maxFile);
  mp3_play(question);
  delay(delayQuestion[question]);
}

void answerQuestion() {
  char typeKey = kypd.getKey();
  if(typeKey) {
    if(typeKey == '#') {
      answer = "";
      lcdCursor = 0;
      lcd.clear();
      lcd12("Jawaban kamu","");
    } else {
      if(lcdCursor == 16) {
        answer = "";
        lcdCursor = 0;
        lcd.clear();
        lcd12("Jawaban kamu","");
      }
      lcd.setCursor(lcdCursor,1);
      lcd.print(typeKey);
      answer = answer+typeKey;
      lcdCursor++; 
    }
  }

  int statusRead = digitalRead(buttonRead);
  if(statusRead == LOW) {
    float answerFloat = keyQuestion[question].toFloat();
    switch(question) {
      case 2:
      answerCheck(answerFloat);
      break;

      case 3:
      answerCheck(answerFloat);
      break;

      case 4:
      answerCheck(answerFloat);
      break;

      case 5:
      answerCheck(answerFloat);
      break;

      case 7:
      answerCheck(answerFloat);
      break;

      case 8:
      answerCheck(answerFloat);
      break;

      case 9:
      answerCheck(answerFloat);
      break;

      case 10:
      answerCheck(answerFloat);
      break;

      case 12:
      answerCheck(answerFloat);
      break;

      case 13:
      answerCheck(answerFloat);
      break;

      case 14:
      answerCheck(answerFloat);
      break;

      case 15:
      answerCheck(answerFloat);
      break;

      case 17:
      answerCheck(answerFloat);
      break;

      case 18:
      answerCheck(answerFloat);
      break;

      case 19:
      answerCheck(answerFloat);
      break;

      case 20:
      answerCheck(answerFloat);
      break;
    }
  }

  if(answerTry == 3) {
    answering = false;
  }
}

void answerCheck(float key) {
  float answerInt = answer.toFloat();
  if(answerInt == key) {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(1500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(1500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    digitalWrite(ledG, HIGH);
    mp3_play(22);
    delay(3000);
    digitalWrite(ledG, LOW);
    answering = false;
    lcdCursor = 0;
    answer = "";
    note = 1;
  } else if((answerInt >= (key-10))&&(answerInt <= (key+10))) {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(1500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(1500);
    digitalWrite(ledY, HIGH);
    mp3_play(23);
    delay(3000);
    lcd12("Jawaban kamu","                ");
    digitalWrite(ledY, LOW);
    answerTry++;
    lcdCursor = 0;
    answer = "";
    note = 0;
  } else if((answerInt < (key-10))||(answerInt > (key+10))) {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(1500);
    digitalWrite(ledR, HIGH);
    mp3_play(24);
    delay(3000);
    lcd12("Jawaban kamu","                ");
    digitalWrite(ledR, LOW);
    answerTry++;
    lcdCursor = 0;
    answer = "";
    note = 0;
  }
}
