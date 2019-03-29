#include <LiquidCrystal.h>

const int LEFT_PLATE = A0;  // Pin connected to FSR/resistor divider
const int RIGHT_PLATE = A1; // Pin connected to FSR/resistor divider
const int threshold = 5;
float force;
const float VCC = 4.98;     // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
long startTime;
bool left;
bool right;
bool done;
int leftTime;
int rightTime;
long tempTime;
bool leftUp;
bool rightUp;
bool fault;  
const int BUZZER = 8;
const int YELLOW_L = 13;
const int YELLOW_R = 12;
const int ORANGE = 11;
const int GREEN = 10;
const int RED_L = 9;
const int RED_R = A5;
int allPins[] = {YELLOW_L, YELLOW_R, ORANGE, GREEN, RED_L, RED_R};
int yellow[] = {YELLOW_L, YELLOW_R};
int orange[] = {ORANGE};
int red[] = {RED_L, RED_R};
int green[] = {GREEN};
int arrSize;
const int PACE = 300;

LiquidCrystal lcd(2,3,4,5,6,7);

void setup()
{
  pinMode(LEFT_PLATE, INPUT);
  pinMode(RIGHT_PLATE, INPUT);
  pinMode(YELLOW_L, OUTPUT);
  pinMode(YELLOW_R, OUTPUT);
  pinMode(ORANGE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED_L, OUTPUT);
  pinMode(RED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  set_pins(allPins, 6, LOW);
  lcd.begin(16, 2);
  Serial.begin(9600);
  fault = false;
  intro_sequence();
}

void loop()
{
  leftTime = 0;
  rightTime = 0;
  leftUp = false;
  rightUp = false;
  
  Serial.println("Before Getting Ready");
  postLcdMessages("Welcome to", "  the CHUGGER");
  start_sequence();
  // Getting Ready
  Serial.println("After Getting Ready");
  postLcdMessages("Put your drinks", "on to start");
  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);

    if (left == true){
      digitalWrite(YELLOW_L, HIGH); 
      postLcdMessages("Player 1 ready","");
    }
    else
    {
      digitalWrite(YELLOW_L, LOW); 
    }

    if (right == true){
      digitalWrite(YELLOW_R, HIGH); 
      postLcdMessages("Player 2 ready","");
    }
    else
    {
      digitalWrite(YELLOW_R, LOW); 
    }
    
    printStatus(left, right);
    delay(10);
    if ((left == true) && (right == true))
    {
      done = true;
    }
    else
    {
      done = false;
    }
  } while (done == false);

  Serial.println("Get ready");
  postLcdMessages("Drink on Green", "Wait for it");
  delay(5000);

  start_sequence();

  Serial.println("Go");
  startTime = millis();

  done = false;

  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);

    printStatus(left, right);
    Serial.println("Left Time: " + String(leftTime));
    Serial.println("Right Time: " + String(rightTime));

    tempTime = millis();

    String elapsedTime = convertMills(tempTime - startTime);
    postLcdMessages("Drink!", elapsedTime + " seconds");
    
    Serial.println("  LeftUp: " + String(leftUp));
    if (leftUp == false)
    {
      if (left == false)
      {
        leftUp = true;
      }
    }
    else
    {
      if (leftTime == 0)
      {
        if (left == true)
        {
          leftTime = tempTime - startTime;
        }
      }
    }
    Serial.println("  RightUp: " + String(rightUp));

    if (rightUp == false)
    {
      if (right == false)
      {
        rightUp = true;
      }
    }
    else
    {
      if (rightTime == 0)
      {
        if (right == true)
        {
          rightTime = tempTime - startTime;
        }
      }
    }
    //delay(500);
    delay(10);
    if ((leftTime != 0) && (rightTime != 0))
    {
      Serial.println("     check before complete" + String(leftTime) + ", " + String(rightTime));
      done = true;
    }
    else
    {
      Serial.println("     check before not complete" + String(leftTime) + ", " + String(rightTime));
      done = false;
    }
  } while (done == false);

  Serial.println("Left time = " + convertMills(leftTime));
  Serial.println("Right time = " + convertMills(rightTime));

  set_winner_leds();

  
  Serial.println("---Enjoying some time until next round");
  
  delay(1000);
  
  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);
    printStatus(left, right);
    delay(100);
    if ((left == false) && (right == false))
    {
      done = true;
    }
    else
    {
      done = false;
    }
  } while (done == false);
  Serial.println("------both drinks off the pads. Ready to go again");

}

//----------------------------------------------------------

void set_pins(int pins[], int num, int setting){
  for(int i = 0;i < num; i++){  
    digitalWrite(pins[i], setting); 
   }
}

void intro_sequence(){
    flash_sequence();
    cycle_sequence();
    cycle_sequence();
    cycle_sequence();
    flash_sequence();
}

void start_sequence(){
    long startSeqTime = millis();
    bool startFlag = false;
    int flashCount = 0;
    int duration, flashcount;
    
    set_pins(orange, 1, HIGH);
    do
    {
      Serial.println("FlashCount: " + String(flashCount));
      
      left = itemOn(LEFT_PLATE);
      right = itemOn(RIGHT_PLATE);

      if (left == false) {
        fault_sequence("LEFT");
      }
      if (right == false) {
        fault_sequence("RIGHT");
      }

      duration = millis() - startSeqTime;
      Serial.println("  Duration: " + String(duration));
      Serial.println("  Duration/PACE*2: " + String(duration/(PACE*2)));
      
      flashcount = duration/(PACE * 2);

      Serial.println("    Mod: " + String(duration % (PACE/2)));
      
      if ((duration % (PACE *2)) < PACE) {
        set_pins(orange, 1, HIGH);
      }
      else {
        set_pins(orange, 1, LOW);
      }

      if (flashcount == 3) {
        startFlag = true;
      }
      
      } while (startFlag == false);
      
    set_pins(orange, 1, LOW); 
    set_pins(green, 1, HIGH);

}

void fault_sequence(String side){
  fault = true;
  set_pins(allPins, 6, LOW);
  int sidePin;
  if (side == "LEFT") {
    sidePin = RED_L;
    leftUp = true;
    leftTime = -1;
  } else {
    sidePin = RED_R;  
    rightTime = -1;
    rightUp = true;
  }
  for (int i = 0; i < 12; i++) {
    digitalWrite(sidePin, HIGH); 
    //BUZZER on
    tone(BUZZER, 3000);
    delay(PACE/2);
    digitalWrite(sidePin, LOW); 
    noTone(BUZZER);
    delay(PACE/2);
  }
  digitalWrite(sidePin, HIGH); 
}

void flash_sequence(){
    set_pins(allPins, 6, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    delay(PACE);
    set_pins(allPins, 6, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    delay(PACE);
    set_pins(allPins, 6, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    delay(PACE);
    set_pins(allPins, 6, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    delay(PACE);
    set_pins(allPins, 6, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
}

void cycle_sequence(){
  set_pins(yellow, 2, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    set_pins(orange, 1, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    set_pins(green, 1, HIGH);
    delay(PACE);
    set_pins(allPins, 6, LOW);
    set_pins(red, 2, HIGH);
    delay(PACE);
}

// after everything
void set_winner_leds(){
  if (fault == true){
    set_pins(allPins, 6, LOW);
    if (leftTime == -1){
      set_pins(yellow[1], 1, HIGH);
      set_pins(red[0], 1, HIGH);
      postLcdMessages("Player 2 wins!","");
      delay(1000);
      postLcdMessages("Player 1 cheats " , "Player 2: " + convertMills(rightTime));
    }
    else if (rightTime == -1) {
      set_pins(yellow[0], 1, HIGH);
      set_pins(red[1], 1, HIGH);
      postLcdMessages("Player 1 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2 cheats" );
    }
    else {
      set_pins(red, 2, HIGH);
      postLcdMessages("Everybody cheats!","And got caught");
    }
  }
  else {
    if (leftTime == rightTime){
      set_pins(yellow, 2, HIGH);
      postLcdMessages("A Tie!!", "");
      delay(500);
      postLcdMessages("A Tie!!", "You both suck");
    }
    else if (leftTime < rightTime) {
      set_pins(yellow[0], 1, HIGH);
      postLcdMessages("Player 1 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2: " + convertMills(rightTime));
    }
    else {
      set_pins(yellow[1], 1, HIGH);
      postLcdMessages("Player 2 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2: " + convertMills(rightTime));
    }
  }
}


void printStatus(bool left, bool right)
{
  Serial.print(left);
  Serial.print(" ");
  Serial.print(right);
  Serial.print(";");
  Serial.print("\n");
}

bool itemOn(int pin)
{
  force = calculateForce(pin);
  if (force > threshold)
  {
    return false;
  }
  else
  {
    return true;
  }
}

String convertMills(long milliseconds)
{
  float seconds = milliseconds / 1000.0;
  return String(seconds);
}

float calculateForce(int pin)
{
  int fsrADC = analogRead(pin);
  if (fsrADC != 0) // If the analog reading is non-zero
  {
    float fsrV = fsrADC * VCC / 1023.0;
    float fsrR = R_DIV * (VCC / fsrV - 1.0);
    float fsrG = 1.0 / fsrR; // Calculate conductance
    if (fsrR <= 600)
      force = (fsrG - 0.00075) / 0.00000032639;
    else
      force = fsrG / 0.000000642857;
  }
  else
  {
    force = 0;
  }
  return force;
}

void clearRow(int row){
  lcd.setCursor(0, row);
  lcd.write("                ");
}

void postLcdMessages(String row1, String row2){
    clearRow(0);
    clearRow(1);
    lcd.setCursor(0,0);
    lcd.write(row1);
    lcd.setCursor(0,1);
    lcd.write(row2);
}


