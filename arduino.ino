#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

unsigned long time;
bool firstRound;
float maxValue = 0;  //need to be defined out of loop
float maxValue_top = 0;
bool weightMode,rawMode,forceMode;
int modeLength = 6;
bool test = false;  //TODO: create real test version

// int i = 0;
// const byte number_of_samples = 80;
// float mean[number_of_samples];

String addSpace(int number, int num_of_spaces = 10)
{
  String string_number = String(number);
  int length = string_number.length();
  int spaces = num_of_spaces - length;

  for (size_t i = 0; i < spaces; i++)
  {
    string_number += " ";
  }
  return string_number;
}

void setup() {
  Serial.begin(9600);
  lcd.init(); // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Measurement Unit");
  lcd.setCursor(0, 1);
  lcd.print("GT Lorraine");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);


  firstRound=true; //identifies if it is the first loop (! do not change !)
  weightMode=true; //defines starting mode

  //setup mode for first round
  if (weightMode == true)
  {
    forceMode = true;
    weightMode = false;
    rawMode = false;
  }
  else if (rawMode == true)
  {
    forceMode = false;
    weightMode = true;
    rawMode = false;
  }
  else if (forceMode == true)
  {
    forceMode = false;
    weightMode = false;
    rawMode = true;
  }
  
}



void setMode(String mode,String unit, bool specific_configuration=false){

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode:");
  lcd.setCursor(5, 0);
  lcd.print(mode);
  lcd.setCursor(5, 1);
  lcd.print("in ");
  lcd.setCursor(8, 1);
  lcd.print(unit);


  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);

  if (specific_configuration==false)
  {
    lcd.print(mode);
    lcd.setCursor(mode.length(), 0);
    lcd.print(": ");
    modeLength = mode.length() + 1;
  }
  
}




void loop() {
  Serial.flush();
  time = millis();
  
  int sensorValue[3];
  sensorValue[0] = analogRead(A0);   //base sensor input
  sensorValue[1] = analogRead(A1);   //top right sensor input
  sensorValue[2] = analogRead(A2);   //top left sensor input
  // sensor4Value = analogRead(A3);   //sensor input
  int sensorButtonReset = digitalRead(2);
  int sensorButtonMode = digitalRead(4);

  float voltage[3], resistance[3],conductance[3],force[3],weight[3],tolerance[2];

  for (int i = 0; i < 3; i++)
  {
    voltage[i] = sensorValue[i] * (5.0 / 1023.0);       // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    resistance[i] = 10000.0 * (5.0 / (voltage[i])-1.0); //Pont diviseur de tension
    conductance[i] = 1.0 / resistance[i];
    float k = 10000.0; // scale factor
    force[i] = k * conductance[i];
  }

  force[1]=(force[1]+force[2])/2;
  
//CALIBRATIONS

  double a = 2.23805E-01;  //calibration factor (found using curve fit and weights)
  double b = 0;     //calibration constant (found using curve fit and weights). corresponds to weight of initial plate + uncertainties
  force[0] = a * force[0] + b; //calibrated force sensor base

  a = 4.85419E+00; //calibration factor (found using curve fit and weights)
  b = 0;  
  force[1] = a * force[1] + b; //calibrated force sensors top

  // EQUATIONS DEPENDANT ON CALIBRATIONS

  

  tolerance[0] = 9E-02; //TOLERANCE in Newtons
  tolerance[1] = 1.8E-02; //TOLERANCE in Newtons

  if (test == false)
  {
    for (size_t i = 0; i < 2; i++)
    {
      if (force[i] < tolerance[i])
      {
        if (force[i] < 0)
        {
          force[i] = 0;
        }
        else if (force[i] < -tolerance[i])
        {
          force[i] = -34404;
        }
        else
        {
          force[i] = 0;
        }
      }
    }
    
  }

  for (int i = 0; i < 3; i++)
  {
    weight[i] = force[i] * 1E3 / 9.81; //weight in grams
  }
  


  //BUTTON SELECTION FUNCTION
  if (sensorButtonMode || firstRound){ 
    firstRound=false;
    if (weightMode == true){
  //RAW MODE
      weightMode = false;
      rawMode = true;
      forceMode = false;

      setMode("Raw", "pt(0-1023)",true);

      lcd.print("t1:");
      modeLength = 3;
      lcd.setCursor(7, 0);
      lcd.print("t2:");
      lcd.print(0.0);
      lcd.setCursor(0, 1);
      lcd.print("b:");
      lcd.setCursor(3, 1);
      lcd.print(0.0);
      lcd.print("            ");
    } else if (rawMode==true){
  //FORCE MODE
    weightMode = false;
    rawMode = false;
    forceMode = true;

    setMode("Force","Newtons");

    lcd.setCursor(0, 1);
    lcd.print("MaxForce:");
    maxValue = 0;
    lcd.setCursor(9, 1);
    lcd.print(maxValue);
    lcd.print("            ");

    } else if (forceMode==true){
      
  //WEIGHT MODE
      weightMode = true;
      rawMode = false;
      forceMode = false;

      setMode("Weight", "g",true);
      lcd.print("T:");
      modeLength = 2;
      lcd.setCursor(9, 0);
      lcd.print("Tm:");
      lcd.print(0.0);
      lcd.setCursor(0, 1);
      lcd.print("B:");
      lcd.print(0.0);
      lcd.setCursor(9, 1);
      lcd.print("Bm:");
      lcd.print(0.0);
      lcd.print("            ");
    }
  
}

//MODE FUNCTIONS: while running the loop

    if (weightMode)
    {
  //WEIGHT MODE
  lcd.setCursor(2, 0);
  lcd.print(addSpace(weight[1],7));
  lcd.setCursor(2, 1);
  lcd.print(addSpace(weight[0],7));

  if (sensorButtonReset)
  { //reset maxValue to 0
    maxValue = 0;
    maxValue_top = 0;

    lcd.setCursor(12, 1);
    lcd.print(maxValue);
    lcd.print("        ");
    lcd.setCursor(12, 0);
    lcd.print(maxValue_top);
    lcd.print("        ");
    }

    if (weight[0] > maxValue)   //peak weight base
    {
      maxValue = weight[0];
      lcd.setCursor(12, 1);
      lcd.print(maxValue);
      lcd.print("            ");
    }

    if (weight[1] > maxValue_top) //peak weight base
    {
      maxValue_top = weight[1];
      lcd.setCursor(12, 0);
      lcd.print(maxValue_top);
      lcd.print("            ");
    }

    }

    else if (rawMode)
    {


  //RAW MODE
  lcd.setCursor(0,0);
  lcd.print("t1:");
  lcd.print(addSpace(sensorValue[1],4));

    lcd.setCursor(7, 0);
  lcd.print("t2:");
  lcd.print(addSpace(sensorValue[2],4));
  lcd.setCursor(3, 1);
  lcd.print(addSpace(sensorValue[0], 12));
    }
    else if (forceMode)
    {
  //FORCE MODE
      lcd.setCursor(modeLength, 0);
      // lcd.print(force[0],6);// for debugging
      lcd.print(force[0],2);
      lcd.print("            ");

      if (sensorButtonReset)
      { //reset maxValue to 0
        maxValue = 0;
        lcd.setCursor(9, 1);
        lcd.print(maxValue,2);
        lcd.print("        ");
      }
      if (force[0] > maxValue)
      {
        maxValue = force[0];
        lcd.setCursor(9, 1);
        lcd.print(maxValue,2);
        lcd.print("            ");
      }
    }


//* SERIAL PRINTING
    Serial.print(time);
    Serial.print("\t");
    // Serial.print(sensorValue[0]);
    Serial.print(weight[0]);
    Serial.print("\t");
    // Serial.print(sensorValue[1]);
    Serial.println(weight[1]);
    // Serial.print("\t");
    // Serial.println(sensorValue[2]);
    // Serial.println(sensorValue[2]);

    // Serial.println(voltage);
  
}