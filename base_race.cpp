
const int LEFT_PLATE = A0; // Pin connected to FSR/resistor divider
const int RIGHT_PLATE = A1; // Pin connected to FSR/resistor divider
const int threshold = 5;
float force;
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
int startTime;
bool left;
bool right;
bool done;
int leftTime;
int rightTime;
int tempTime;
bool leftUp;
bool rightUp;

void setup() {
  pinMode(LEFT_PLATE, INPUT);
  pinMode(RIGHT_PLATE, INPUT);
  Serial.begin(9600);
}

void loop() {
  leftTime = 0;
  rightTime = 0;

  do
  {
    left = itemOn(LEFT_PLATE);
    right = itemOn(RIGHT_PLATE);
    printStatus(left, right);
    delay(100);
    if ((left == true) && (right == true)){
      done = true;
    } else
    {
      done = false;
    }  
  } while (done == false);

  Serial.println("Get ready");

  delay(1000);
  
  Serial.println("Go");
  startTime = millis();
  leftUp = false;
  rightUp = false;

// check each one separately to make sure it goes to false
// for now, just delay and assume that drinks are up




  done = false;
  
  do
    {
      left = itemOn(LEFT_PLATE);
      right = itemOn(RIGHT_PLATE);

      
      printStatus(left, right);
      Serial.println(leftTime);
      Serial.println(rightTime);
  
      tempTime = millis();
      
      if (leftTime == 0){
        if (left == true) {
          leftTime = tempTime - startTime;
        }
      }
  
      if (rightTime == 0){
        if (right == true) {
          rightTime = tempTime - startTime;
        }
      }
      
      delay(10);
      
      if ((leftTime != 0) && (rightTime != 0)){
        done = true;
      } else
      {
        done = false;
      }  
    } while (done == false);

  Serial.println("Left time = " + convertMills(leftTime));
  Serial.println("Right time = " + convertMills(rightTime));

  delay(1000);
}

//----------------------------------------------------------

void printStatus(bool left, bool right){
  Serial.print(left);
  Serial.print(" ");
  Serial.print(right);
  Serial.print(";");
  Serial.print("\n");
}

bool itemOn(int pin){
  force = calculateForce(pin);
  if (force > threshold){
    return false;
    }
  else {
    return true;
    }
}

String convertMills(long milliseconds){
  float seconds = milliseconds/1000.0;
  return String(seconds);
}

float calculateForce(int pin){
  int fsrADC = analogRead(pin);
  if (fsrADC != 0) // If the analog reading is non-zero
  {
    float fsrV = fsrADC * VCC / 1023.0;
    float fsrR = R_DIV * (VCC / fsrV - 1.0);
    float fsrG = 1.0 / fsrR; // Calculate conductance
    if (fsrR <= 600) 
      force = (fsrG - 0.00075) / 0.00000032639;
    else
      force =  fsrG / 0.000000642857;
  }
  else
  {
  force = 0;
  }
  return force;
}
