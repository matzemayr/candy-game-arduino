#include <rgb_lcd.h>
#include <Wire.h>


rgb_lcd lcd;

const int buttonPin = 5;
const int touchPin = 6;
const int rotatePin = 7;

int colorR = 0;
int colorG = 0;
int colorB = 0;

int buttonValue = 0;
int touchValue = 0;
int rotateValue = 0;

boolean initialPressed = false;
int pressMillis = 0;

int lastStepMillis = 0;
int countDownState = 0;
int randomDelay;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

byte arrowDown[8] = {
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte arrowUp[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b11111,
};

int selection = 0;


void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(touchPin, INPUT);
  pinMode(buttonPin, INPUT);

  randomSeed(analogRead(0));
  
  lcd.createChar(0, arrowDown);
}

void loop() {
  buttonValue = digitalRead(buttonPin);
  touchValue = digitalRead(touchPin);
  rotateValue = digitalRead(rotatePin);


  lcd.setRGB(colorR, colorG, colorB);
  lcd.clear();

  selection = map(rotateValue, 0, 1023, 0, 1);


  if(countDownState == 0){
    lcd.setCursor(0, 0);
    lcd.print(String(scorePlayer1) + " Press -> start");

    lcd.setCursor(0, 1);
    lcd.print(String(scorePlayer2) + " Hold  -> reset");
    
    if(buttonValue + touchValue > 0 && !initialPressed){
      pressMillis++;
      Serial.print(pressMillis);
      if(pressMillis > 20){
        reset();
      }
    }else if(buttonValue + touchValue == 0){
      if(pressMillis > 0){
        startCountDown();
      }else{
        initialPressed = false;
      }
    }
  }else{
    //game start
    
    //always show score during game
    lcd.setCursor(0, 0);
    lcd.print(scorePlayer1);

    lcd.setCursor(0, 1);
    lcd.print(scorePlayer2);

    if(countDownState == 1 && (millis() > lastStepMillis + 1000)){
      setColor("yellow");
      countDownState = 2;
      lastStepMillis = millis();
    }else if(countDownState == 2 && millis() > (lastStepMillis + randomDelay)){
      setColor("green");
      countDownState = 3;
    }else if(countDownState == 3){
      if(buttonValue == 1){
        scorePlayer1++;
        nextRound();
      }else if(touchValue == 1){
        scorePlayer2++;
        nextRound();
      }  
    }
    
  }
  
  // to slow down for LCD
  delay(100);
}

void startCountDown(){
  Serial.println("countdown started");
  lastStepMillis = millis();
  countDownState = 1;
  int seconds = random(10);
  int ms = random(255) * 4;
  randomDelay = (seconds * 1000) + ms;
  setColor("red");
}

void nextRound(){
  lastStepMillis = millis();
  countDownState = 0;
  pressMillis = 0;
  initialPressed = true;
  setColor("black");
  //todo prevent immediate restart within same button press with delay after rendering black background
}

void reset(){
  scorePlayer1 = 0;
  scorePlayer2 = 0;
  nextRound();
}

void setColor(String c){
  if(c == "red"){
    colorR = 255,
    colorG = 0;
    colorB = 0;
  }else if(c == "yellow"){
    colorR = 255;
    colorG = 255;
    colorB = 0; 
  }else if(c == "green"){
    colorR = 0;
    colorG = 255;
    colorB = 0;  
  }else if(c == "black"){
    colorR = 0;
    colorG = 0;
    colorB = 0;  
  }
  
}
