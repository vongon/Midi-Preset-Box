/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <MF104.h>
#include <EEPROM.h>

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
//decalre MF104 object
#define totalParameters 19

MF104 superDelay[3][3];
int GlobalBank = 1; //start on bank 1
int GlobalPreset = 1; //start on preset 1
int GlobalParameter = 1; //start on parameter 1
int GlobalValue;


// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();



void setup() {
  Serial.begin(31250);
  lcd.begin(16, 2);
  lcd.setBacklight(VIOLET);
  initializePins(4,10,8);
  loadPreSets();  
  GlobalValue = superDelay[GlobalBank][GlobalPreset].getValue(GlobalParameter);
  updateLCD(GlobalBank, GlobalPreset, GlobalParameter, GlobalValue);
}

uint8_t i=0;
int prevMidiValue;
int prevBS1;
int prevBS2;
int prevBS3;

void loop() {
  uint8_t buttons = lcd.readButtons();
  GlobalValue = readPot(A0); 
  int buttonState1 = digitalRead(4);
  int buttonState2 = digitalRead(10);
  int buttonState3 = digitalRead(8);
  
  
  if (GlobalValue != prevMidiValue) //Knob is changed
    {
      //lcd.clear();
      updateLCDvalue(GlobalValue);
      superDelay[GlobalBank][GlobalPreset].setValue(GlobalParameter,GlobalValue);
      superDelay[GlobalBank][GlobalPreset].sendSettings();
      prevMidiValue = GlobalValue;
      delay(50);
    }

  if (buttons) //Selection Buttons are Changed
  {
    if (buttons == 0x12) //LEFT + RIGHT Pressed
    {
      GlobalBank++;
      if(GlobalBank > 3)
        GlobalBank = 1;
      GlobalValue = superDelay[GlobalBank][GlobalPreset].getValue(GlobalParameter);
      updateLCD(GlobalBank, GlobalPreset, GlobalParameter, GlobalValue);
      delay(400);
    }
    else if (buttons & BUTTON_RIGHT) {
      GlobalParameter--;
      if(GlobalParameter < 1) //check for edge
        GlobalParameter = totalParameters;
      GlobalValue = superDelay[GlobalBank][GlobalPreset].getValue(GlobalParameter);
      updateLCD(GlobalBank, GlobalPreset, GlobalParameter, GlobalValue);
      delay(200);
    }
    else if (buttons & BUTTON_LEFT) {
      GlobalParameter++;
      if(GlobalParameter > totalParameters) //check for edge
        GlobalParameter = 1;
      GlobalValue = superDelay[GlobalBank][GlobalPreset].getValue(GlobalParameter);
      updateLCD(GlobalBank, GlobalPreset, GlobalParameter, GlobalValue);
      delay(200);
    }
    else if (buttons & BUTTON_SELECT) {
      storePreset(GlobalBank, GlobalPreset);
      //superDelay[GlobalBank][GlobalPreset].sendSettings();
      lcd.clear();
      lcd.setCursor(4,0); lcd.print("SAVED");
      lcd.setCursor(3,1); lcd.print("SETTING!!!");
      delay(1000);
      updateLCD(GlobalBank, GlobalPreset, GlobalParameter, GlobalValue);
    }
    
  }

  //store "previous" states so we can detect changes

  prevBS1 = buttonState1;
  prevBS2 = buttonState2;
  prevBS3 = buttonState3;
}

void storePreset(int bank, int preset)
{
  int address = (bank*100) + (preset*20);
  superDelay[bank][preset].storeData(address);
}

void loadPreSets() //load all presets from EEPROM
{
  superDelay[0][0].loadData(0);
  superDelay[0][1].loadData(20);
  superDelay[0][2].loadData(40);
  superDelay[1][0].loadData(100);
  superDelay[1][1].loadData(120);
  superDelay[1][2].loadData(140);
  superDelay[2][0].loadData(200);
  superDelay[2][1].loadData(220);
  superDelay[2][2].loadData(240);
}

void initializePins(int A, int B, int C)
{
     pinMode(A, INPUT);
     pinMode(B, INPUT);
     pinMode(C, INPUT);
}


int readPot(int port)
{
   int sensorValue = analogRead(port);
   return sensorValue * (127.0/1023.0);
}

void updateLCD(int bank, int preset, int parameter, int value)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(bank); lcd.print("-");
    lcd.print(preset); lcd.print(":"); 
    printParameterName(parameter);
    lcd.setCursor(0,1);
    lcd.print("VALUE:"); lcd.print(value);
}

void updateLCDvalue(int value)
{
    lcd.setCursor(6,1);
    lcd.print(value);
    lcd.print("  ");
}

void printParameterName(int parameter)
{ 
  switch(parameter)
  {
    case 1:
      lcd.print("Output Level");
      break;
    case 2:
      lcd.print("Time");
      break;
    case 3:
      lcd.print("Feedback");
      break;
    case 4:
      lcd.print("Mix");
      break;
    case 5:
      lcd.print("LFO Rate");
      break;
    case 6:
      lcd.print("LFO Amount");
      break;
    case 7:
      lcd.print("LFO Waveform");
      break;
    case 8:
      lcd.print("Range");
      break;
    case 9:
      lcd.print("Time Slew Rate");
      break;
    case 10:
      lcd.print("EQ");
      break;
    case 11:
      lcd.print("Dly Time Mult");
      break;
    case 12:
      lcd.print("LFO Phase Reset");
      break;
    case 13:
      lcd.print("LFO ClkDiv");
      break;
    case 14:
      lcd.print("Time ClkDiv");
      break;
    case 15:
      lcd.print("Spillover");
      break;
    case 16:
      lcd.print("Triplets");
      break;
    case 17:
      lcd.print("Tap Dest");
      break;
    case 18:
      lcd.print("LED Div");
      break;
    case 19:
      lcd.print("Bypass");
      break;
  }  
}
