#include <LiquidCrystal.h>

const int LEFT_PLATE = A0;  // Pin connected to FSR/resistor divider
const int RIGHT_PLATE = A1; // Pin connected to FSR/resistor divider
const int YELLOW_L = 13;
const int YELLOW_R = 12;
const int ORANGE = 11;
const int GREEN = 10;
const int RED_L = 9;
const int RED_R = A5;
const int BUZZER = 8;

const int threshold = 5;
float force;
const float VCC = 4.98;     // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
const int PACE = 200;

unsigned long startTime;

bool left;
bool right;
bool done;
long tempTime;
bool leftUp;
bool rightUp;
bool left_fault;
bool right_fault;

int arrSize;
int leftTime;
int rightTime;

int allPins[] = {YELLOW_L, YELLOW_R, ORANGE, GREEN, RED_L, RED_R};
int yellow[] = {YELLOW_L, YELLOW_R};
int orange[] = {ORANGE};
int red[] = {RED_L, RED_R};
int green[] = {GREEN};

LiquidCrystal lcd(2,3,4,5,6,7);

void setup()
{
  Serial.begin(9600);
  
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
  
  postLcdMessages("Welcome to", "  the CHUGGER!");
  intro_sequence(3);
}

void loop()
{
  postLcdMessages("Welcome to", "  the CHUGGER!");

  leftTime = 0;
  rightTime = 0;
  leftUp = false;
  rightUp = false;
  left_fault = false;
  right_fault = false;

  set_pins(allPins, 6, LOW);
  
  Serial.println("Getting Ready");
  postLcdMessages("Put your drinks", "on to start");
  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);
    
    printStatus(left,right);
    
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
      postLcdMessages("", "Player 2 ready");
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
  postLcdMessages("Drink on Green", "   Wait for it!");
  delay(2000);
  
//  for (int i = 4 ; i = 0; i--){
//    postLcdMessages("Green flashes", "Staring in " + String(i + 1));
//    delay(1000);
//  }
    postLcdMessages("Flash series", "Starting in 5");
    delay(1000);
    postLcdMessages("Flash series", "Starting in 4");
    delay(1000);
    postLcdMessages("Flash series", "Starting in 3");
    delay(1000);
    postLcdMessages("Flash series", "Starting in 2");
    delay(1000);
    postLcdMessages("Flash series", "Starting in 1");
    delay(1000);
  
  start_sequence();

  Serial.println("Go");
  startTime = millis();
  postLcdMessages("Drink it!", "START  START");

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
    //delay(10);
    
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

  Serial.println("I think we are done");
  set_pins(allPins, 6, LOW);
  set_winner_leds();

  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);
    printStatus(left, right);
    delay(10);
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

void intro_sequence(int times){
    flash_sequence();
    for (int i; i < times; i++){
      cycle_sequence();
    }
    flash_sequence();
}

// this is drag racing. check for cheaters
void start_sequence(){
  Serial.println("debugging start_sequence");
    long startSeqTime = millis();
    bool startFlag = false;
    int flashCount = 0;
    int duration, flashcount;
    
    set_pins(orange, 1, HIGH);
    do
    { 
      if (left_fault == false){     
        left = itemOn(LEFT_PLATE);
        if (left == false) {
          fault_sequence("LEFT");
          left_fault = true;
        }
      }

      if (right_fault == false){
        right = itemOn(RIGHT_PLATE);
        if (right == false) {
          fault_sequence("RIGHT");
          right_fault = true;
        }
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
    Serial.println("end of start sequence ------");
}


void fault_sequence(String side){
  Serial.println("Start Fault Sequence");
  set_pins(allPins, 6, LOW);
  int sidePin;
  if (side == "LEFT") {
    sidePin = RED_L;
    leftUp = true;
  } else {
    sidePin = RED_R;  
    rightUp = true;
  }
  
  //rewrite to not block
//  for (int i = 0; i < 12; i++) {
//    digitalWrite(sidePin, HIGH); 
//    //BUZZER on
//    tone(BUZZER, 300);
//    delay(PACE/2);
//    digitalWrite(sidePin, LOW); 
//    noTone(BUZZER);
//    delay(PACE/2);
//  }
  digitalWrite(sidePin, HIGH); 
  Serial.println("End Fault Sequence");
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
  Serial.println("Start Set Winner");
    set_pins(allPins, 6, LOW);
    if ((left_fault == true) && (right_fault == false)){
      // player 1 faulted and player 2 wins with a score
      digitalWrite(YELLOW_R, HIGH); 
      digitalWrite(RED_L, HIGH); 
      postLcdMessages("Player 2 wins!","");
      delay(1000);
      postLcdMessages("Player 1 cheats " , "Player 2: " + convertMills(rightTime));
    }
    
    else if ((left_fault == false) && (right_fault == true)){
      // player 2 faults and player 2 wins with a score
      digitalWrite(YELLOW_L, HIGH); 
      digitalWrite(RED_R, HIGH); 
      postLcdMessages("Player 1 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2 cheats" );
    }
    
    else if ((left_fault == true) && (right_fault == true)){
      // players 1 and 2 fault
      digitalWrite(RED_L, HIGH); 
      digitalWrite(RED_R, HIGH); 
      postLcdMessages("Everybody cheats!","");
      delay(1000);
      postLcdMessages("Everybody cheats!","And got caught!");
    }
  
    else if (leftTime == rightTime){
      // players 1 and 2 tie
      digitalWrite(YELLOW_L, HIGH); 
      digitalWrite(YELLOW_R, HIGH); 
      postLcdMessages("A Tie!!", "");
      delay(1000);
      postLcdMessages("A Tie! Hmmm", "Score: " + convertMills(leftTime));
    }
    
    else if (leftTime < rightTime) {
      // both complete and player 1 wins
      digitalWrite(YELLOW_L, HIGH); 
      digitalWrite(RED_R, HIGH); 
      postLcdMessages("Player 1 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2: " + convertMills(rightTime));
    }
    
    else {
      // both complete and player 2 wins
      digitalWrite(YELLOW_R, HIGH); 
      digitalWrite(RED_L, HIGH); 
      postLcdMessages("Player 2 wins!","");
      delay(1000);
      postLcdMessages("Player 1: " + convertMills(leftTime), "Player 2: " + convertMills(rightTime));
    }
  
  Serial.println("End Set Winner");
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
  if (force < threshold)
  {
    Serial.println(String(pin) + ": Off");
    return false;
  }
  else
  {
    Serial.println(String(pin) + ": On");
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
    lcd.print(row1);
    lcd.setCursor(0,1);
    lcd.print(row2);
}


