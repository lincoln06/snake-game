#include <LiquidCrystal.h>

const int rs = 12, e = 3, d4 = 5, d5 = 4, d6 = 6, d7 = 2;
const int downBtn = 7;
const int upBtn = 8;
const int rightBtn = 9;
const int leftBtn = 10;

LiquidCrystal lcd(rs, e, d4, d5, d6, d7);


const int numRows = 4;
const int numCols = 20;

int snakeX[80] = {10};
int snakeY[80] = {2};
int snakeLength = 1;

int foodX;
int foodY;

int dx = 1;
int dy = 0;

int newDx = 1;
int newDy = 0;

int startLevel = 0;

int startDelay = 220;
unsigned long lastMoveTime = 0;
unsigned long lastJoystickCheckTime = 0;
unsigned long joystickCheckInterval = 50; 

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
  pinMode(downBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
  randomSeed(analogRead(0));
  runNewGame();
}

void loop() {
  unsigned long currentTime = millis();

  // sprawdzanie odchylenia joysticka
  if (currentTime - lastJoystickCheckTime >= joystickCheckInterval) {
    getPosition();
    lastJoystickCheckTime = currentTime;
  }

  // poruszanie wężem
  if (currentTime - lastMoveTime >= startDelay) {
    moveSnake();
    checkFood();
    lcd.clear();
    drawSnake();
    drawFood();
    lastMoveTime = currentTime;
  }
}

void showStartScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wybierz poziom");
  
  while (digitalRead(rightBtn) == HIGH) {
    delay(100);
    lcd.setCursor(9, 3);
    lcd.print(startLevel);
    getPosition();
    switch(newDy) {
      case 1:
        startLevel--;
        newDy = 0;
        break;
      case -1:
        startLevel++;
        newDy = 0;
        break;
    }
    if (startLevel < 0) startLevel = 5;
    if (startLevel > 5) startLevel = 0;
  }

  startDelay = 220 - (startLevel * 10); 
  lcd.setCursor(1, 3);
}

void showGameOverScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Przegrales");
  lcd.setCursor(0, 1);
  lcd.print("Nacisnij przycisk");
  lcd.setCursor(0, 2);
  lcd.print("w prawo");
  lcd.setCursor(0, 3);
  lcd.print("Aby zagrac");
  while (digitalRead(rightBtn) == HIGH) {}

  delay(1000);
}

void runNewGame() {
  startDelay = 220;
  snakeLength = 1;
  snakeX[0] = 10;
  snakeY[0] = 2;
  dx = 1;
  dy = 0;
  newDx = 1;
  newDy = 0;
  showStartScreen();
  spawnFood();
}

void getPosition() {

  if (digitalRead(upBtn) == LOW && dy == 0) {
    newDy = -1;
    newDx = 0;
  } else if (digitalRead(downBtn) == LOW && dy == 0) {
    newDy = 1;
    newDx = 0;
  }

  if (digitalRead(leftBtn) == LOW && dx == 0) {
    newDx = -1;
    newDy = 0;
  } else if (digitalRead(rightBtn) == LOW && dx == 0) {
    newDx = 1;
    newDy = 0;
  }
}

void moveSnake() {
  // zapobieganie "zawróceniu" węża
  if ((newDx != -dx || newDy != -dy) && (newDx != dx || newDy != dy)) {
    dx = newDx;
    dy = newDy;
  }

  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  
  snakeX[0] += dx;
  snakeY[0] += dy;

  // sprawdzanie, czy nie przechodzimy przez ścianę
  if (snakeX[0] < 0) snakeX[0] = numCols - 1;
  if (snakeX[0] >= numCols) snakeX[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = numRows - 1;
  if (snakeY[0] >= numRows) snakeY[0] = 0;

  // sprawdzanie, czy nie zjadamy własnego ogona
  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      showGameOverScreen();
      runNewGame();
      return;
    }
  }
}

void checkFood() {
  // sprawdzanie kolizji z jedzonkiem
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    spawnFood();
  }
}

void spawnFood() {
  bool validPosition = false;
  while (!validPosition) {
    foodX = random(numCols);
    foodY = random(numRows);
    validPosition = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        validPosition = false;
        break;
      }
    }
  }
}

void drawSnake() {
  for (int i = 0; i < snakeLength; i++) {
    lcd.setCursor(snakeX[i], snakeY[i]);
    lcd.print("#");
  }
}

void drawFood() {
  lcd.setCursor(foodX, foodY);
  lcd.print("O");
}
