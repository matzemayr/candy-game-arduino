#include <rgb_lcd.h>
#include <Wire.h>
#include <Servo.h>

rgb_lcd lcd;

//Player1
const int buttonPin = 5;

//Player2
const int touchPin = 6;

const int servoPin = 7;
const int rotatePin = A0;

Servo servoMotor;

int gameState = 0;
//0 = Main Menu
//1 = Game 1
//2 = Game 2

int gameSelection = 0;
int lastSetRotateValue = 0;

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

int game2State = 0;
//0 = menu
//1 = show target number
//2 = count up
//3 = show result
int game2RandomNumber;
int game2DisplayNumber;
int game2Player1Number = 0;
int game2Player2Number = 0;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

int lastWinner = 0;

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


void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(touchPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(rotatePin, INPUT);

  servoMotor.attach(servoPin);

  randomSeed(analogRead(1));

  lcd.createChar(0, arrowDown);

  setColor("white");

  servoMotor.write(60);
}

void loop() {
  buttonValue = digitalRead(buttonPin);
  touchValue = digitalRead(touchPin);
  rotateValue = analogRead(rotatePin);

  Serial.println(rotateValue);
  Serial.println(millis());


  lcd.setRGB(colorR, colorG, colorB);
  lcd.clear();

  Serial.println(gameSelection);
  if(gameState == 0){
    updateMainMenu(rotateValue, buttonValue, touchValue);
  }else if(gameState == 1){
    game1Update();
  }else if(gameState == 2){
    game2Update();  
  }
  
  // to slow down for LCD
  delay(100);
}

void setGameState(int newGameState){
  if((digitalRead(touchPin) + digitalRead(buttonPin)) > 0){
    initialPressed = true;  
  }
  gameState = newGameState;
}

void releaseCandy(){
  if(lastWinner == 1){
    servoMotor.write(100);
  }else{
    servoMotor.write(35);
  }
  delay(500);
  servoMotor.write(60); 
}

void updateMainMenu(int rotateValue, int buttonValue, int touchValue){
    if(abs(rotateValue - lastSetRotateValue) > 100){
      lastSetRotateValue = rotateValue;
      gameSelection = gameSelection == 0 ? 1 : 0;
    }
    if((buttonValue + touchValue) > 0 && !initialPressed){
      setGameState(gameSelection + 1);
    }else if(buttonValue + touchValue == 0){
      initialPressed = false;  
    }
    drawMainMenu();
}

void drawMainMenu(){
  lcd.setCursor(0,1);
  lcd.write("Game1   Game2");

  if((millis() % 500) < 250){
    lcd.setCursor(gameSelection == 0 ? 0 : 8, 0);
    lcd.write((unsigned char)0);
  }
}

void drawGameMenu(){
  lcd.setCursor(0, 0);
  lcd.print("  Press: start ");

  lcd.setCursor(0, 1);
  lcd.print(String(scorePlayer1) + " Hold: menu   " + String(scorePlayer2));
    
  if(buttonValue + touchValue > 0 && !initialPressed){
    pressMillis++;
    Serial.print(pressMillis);
    if(pressMillis > 20){
      reset();
    }
  }else if(buttonValue + touchValue == 0){
    if(pressMillis > 0){
      if(gameSelection == 0){
        startGame1();  
      }else{
        startGame2();
        lastStepMillis = millis();
      }
    }else{
      initialPressed = false;
    }
  }
}

void startGame1(){
  Serial.println("countdown started");
  lastStepMillis = millis();
  countDownState = 1;
  int seconds = random(10);
  int ms = random(255) * 4;
  randomDelay = (seconds * 1000) + ms;
  setColor("red");
}

void game1Update(){
  if(countDownState == 0){
    drawGameMenu();
  }else{
    //game start
    
    //always show score during game
    lcd.setCursor(0, 0);
    lcd.print(String(scorePlayer1) + "            " + String(scorePlayer2));

    //lcd.setCursor(0, 1);
    //lcd.print(scorePlayer2);

    if(countDownState == 1 && (millis() > (lastStepMillis + randomDelay))){
      setColor("green");
      countDownState = 2;
      lastStepMillis = millis();
    }else if(countDownState == 2){
      if(buttonValue == 1){
        scorePlayer1++;
        lastWinner = 1;
        nextRound();
      }else if(touchValue == 1){
        scorePlayer2++;
        lastWinner = 2;
        nextRound();
      }  
    }
    
  }
}

void nextRound(){
  releaseCandy();
  lastStepMillis = millis();
  countDownState = 0;
  pressMillis = 0;
  initialPressed = true;
  setColor("white");
  //TODO: prevent immediate restart within same button press with delay after rendering black background
}

void reset(){
  scorePlayer1 = 0;
  scorePlayer2 = 0;
  setGameState(0);
  nextRound();
}


void startGame2(){
  game2State = 1;
  game2RandomNumber = random(100) + 20;
  lastStepMillis = millis();
  setColor("green");
}

void game2Update(){
  if(game2State == 0){
     drawGameMenu();
  }else if(game2State == 1){
    if((millis() % 1000) < 500){
      lcd.setCursor(0, 0);
      lcd.print("       " + String(game2RandomNumber));
    }

    if(millis() > (lastStepMillis + 3000)){
      game2State = 2;  
      lastStepMillis = millis();
      Serial.print("<<<<<<<<<<MILLLIS SET TO: " + String(millis()) + " >>>>>>>>>>>>>>>>");
    }
  }else if(game2State == 2){
    if(game2DisplayNumber <= 20){
      lcd.setCursor(0,0);
      lcd.print("       " + String(game2DisplayNumber));
    }
    game2DisplayNumber++;
    Serial.print("LASTSTEPMILLIS: " + String(lastStepMillis));
    Serial.print("MILLIS: " + String(millis()));
    //lcd.setPWM(REG_GREEN, alpha);

    if(game2Player1Number == 0 && buttonValue == 1){
      game2Player1Number = game2DisplayNumber;
    }
    if(game2Player2Number == 0 && touchValue == 1){
      game2Player2Number = game2DisplayNumber;  
    }

    if(game2Player1Number != 0 && game2Player2Number != 0){
      game2State = 3;
      lastStepMillis = millis();
      if(abs(game2Player1Number - game2RandomNumber) < abs(game2Player2Number - game2RandomNumber)){
        scorePlayer1++;  
        lastWinner = 1;
      }else{
        scorePlayer2++;  
        lastWinner = 2;
      }
    }
  }else if(game2State == 3){
    if((millis() % 1000) < 500){
      if(abs(game2Player1Number - game2RandomNumber) < abs(game2Player2Number - game2RandomNumber)){
      //player 1 wins
        lcd.setCursor(0,0);
        lcd.print("<--- WINNER!");
      }else{
        //player 2 wins 
        lcd.setCursor(0,0);
        lcd.print("    WINNER! --->");
      }
    }
    lcd.setCursor(0,1);
    lcd.print(" " + String(game2Player1Number) + "          " + String(game2Player2Number));
    if(millis() > lastStepMillis + 5000){
      game2NextRound();
    }
  }
}

void game2NextRound(){
  releaseCandy();
  game2RandomNumber = 0;
  game2DisplayNumber = 0;
  game2Player1Number = 0;
  game2Player2Number = 0;
  pressMillis = 0;
  lastStepMillis = millis();
  pressMillis = 0;
  initialPressed = true;
  setColor("white");
  game2State = 0;
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
  }else if(c=="white"){
    colorR = 255;
    colorG = 255;
    colorB = 255; 
  }
  
}
