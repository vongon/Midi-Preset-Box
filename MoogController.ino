#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <MF104.h>
#include <EEPROM.h>

const int VIOLET = 0x5;
const int BUTTON1_PIN = 4;
const int BUTTON2_PIN = 8;
const int BUTTON3_PIN = 10;
const int LED1_PIN = 5;
const int LED2_PIN = 6;
const int LED3_PIN = 7;
const int RED_BUTTON_PIN = 13;
const int UP_BUTTON_PIN = 12;
const int DOWN_BUTTON_PIN = 11;
const int PLAY_MODE = 1;
const int EDIT_MODE = 2;
const int PRESET1_ADDRESS = 100;
const int PRESET2_ADDRESS = 120;
const int PRESET3_ADDRESS = 140;
const int TOTAL_PARAMETERS = 19;

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
int BUTTON1_PREVIOUS_VALUE;
int BUTTON2_PREVIOUS_VALUE;
int BUTTON3_PREVIOUS_VALUE;
int CURRENT_PRESET;
int CURRENT_PARAM_INDEX;
int CURRENT_PARAM_VALUE;
int PREVIOUS_PARAM_VALUE;
int MODE;

MF104 mf104[4];

void setup() {
  Serial.begin(31250);
  lcd.begin(16, 2);
  lcd.setBacklight(VIOLET);
  
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  BUTTON1_PREVIOUS_VALUE = -1; //Initialize to button1 state
  BUTTON2_PREVIOUS_VALUE = digitalRead(BUTTON2_PIN);
  BUTTON3_PREVIOUS_VALUE = digitalRead(BUTTON3_PIN);
  MODE = PLAY_MODE;
  CURRENT_PRESET = 1;
  CURRENT_PARAM_INDEX = 0;
  mf104[1].loadData(PRESET1_ADDRESS);
  mf104[2].loadData(PRESET2_ADDRESS);
  mf104[3].loadData(PRESET3_ADDRESS);
  printWelcome();
  delay(3000);
  printPerformanceLayout();
}

void loop() { 
  if (MODE == PLAY_MODE) {
    loopPlayMode();
  } else if (MODE == EDIT_MODE) {
    loopEditMode();
  }
}

void loopPlayMode() {
  int button1_value = digitalRead(BUTTON1_PIN);
  int button2_value = digitalRead(BUTTON2_PIN);
  int button3_value = digitalRead(BUTTON3_PIN);

  if (button1_value != BUTTON1_PREVIOUS_VALUE) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    CURRENT_PRESET = 1;
    mf104[CURRENT_PRESET].sendSettings();
    printPerformanceLayout();
  } else if (button2_value != BUTTON2_PREVIOUS_VALUE) {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    CURRENT_PRESET = 2;
    mf104[CURRENT_PRESET].sendSettings();
    printPerformanceLayout();
  } else if (button3_value != BUTTON3_PREVIOUS_VALUE) {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, HIGH);
    CURRENT_PRESET = 3;
    mf104[CURRENT_PRESET].sendSettings();
    printPerformanceLayout();
  } else if (digitalRead(RED_BUTTON_PIN) == 0){
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Edit mode");
    delay(1000);
    lcd.clear();
    printEditLayout();
    MODE = EDIT_MODE;
  } else if (digitalRead(UP_BUTTON_PIN) == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press red button");
    lcd.setCursor(0,1);
    lcd.print("to edit");
    delay(2000);
    printPerformanceLayout();
  } else if (digitalRead(DOWN_BUTTON_PIN) == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press red button");
    lcd.setCursor(0,1);
    lcd.print("to edit");
    delay(2000);
    printPerformanceLayout();
  }

  BUTTON1_PREVIOUS_VALUE = button1_value;
  BUTTON2_PREVIOUS_VALUE = button2_value;
  BUTTON3_PREVIOUS_VALUE = button3_value;
}

void loopEditMode() { 
  blink();

  int button1_value = digitalRead(BUTTON1_PIN);
  int button2_value = digitalRead(BUTTON2_PIN);
  int button3_value = digitalRead(BUTTON3_PIN);

  if (button1_value != BUTTON1_PREVIOUS_VALUE || 
    button2_value != BUTTON2_PREVIOUS_VALUE || 
    button3_value != BUTTON3_PREVIOUS_VALUE ||
    digitalRead(RED_BUTTON_PIN) == 0) {
    if (CURRENT_PRESET == 1) {
      digitalWrite(LED1_PIN, HIGH);
      BUTTON1_PREVIOUS_VALUE = -1;
      BUTTON2_PREVIOUS_VALUE = button2_value;
      BUTTON3_PREVIOUS_VALUE = button3_value;
      mf104[CURRENT_PRESET].storeData(PRESET1_ADDRESS);
    } else if (CURRENT_PRESET == 2) {
      digitalWrite(LED2_PIN, HIGH);
      BUTTON2_PREVIOUS_VALUE = -1;
      BUTTON1_PREVIOUS_VALUE = button1_value;
      BUTTON3_PREVIOUS_VALUE = button3_value;
      mf104[CURRENT_PRESET].storeData(PRESET2_ADDRESS);
    } else if (CURRENT_PRESET == 3) {
      digitalWrite(LED3_PIN, HIGH);
      BUTTON3_PREVIOUS_VALUE = -1;
      BUTTON1_PREVIOUS_VALUE = button1_value;
      BUTTON2_PREVIOUS_VALUE = button2_value;
      mf104[CURRENT_PRESET].storeData(PRESET3_ADDRESS);
    }
    MODE = PLAY_MODE;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Saving...");
    delay(3000);
    lcd.clear();
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
  }
  
  CURRENT_PARAM_VALUE = getKnobValue(A0);
  if (CURRENT_PARAM_VALUE != PREVIOUS_PARAM_VALUE) {
    printValue();
    mf104[CURRENT_PRESET].setValue(CURRENT_PARAM_INDEX, CURRENT_PARAM_VALUE);
    mf104[CURRENT_PRESET].sendSettings();
    PREVIOUS_PARAM_VALUE = CURRENT_PARAM_VALUE;
    delay(50);
  }

  if (digitalRead(UP_BUTTON_PIN) == 0){
    CURRENT_PARAM_INDEX++;
    if (CURRENT_PARAM_INDEX >= TOTAL_PARAMETERS) {
      CURRENT_PARAM_INDEX = 0;
    }
    CURRENT_PARAM_VALUE = mf104[CURRENT_PRESET].getValue(CURRENT_PARAM_INDEX);
    printEditLayout();
    delay(400);
  } else if (digitalRead(DOWN_BUTTON_PIN) == 0){
    CURRENT_PARAM_INDEX--;
    if (CURRENT_PARAM_INDEX < 0) {
      CURRENT_PARAM_INDEX = TOTAL_PARAMETERS - 1;
    }
    CURRENT_PARAM_VALUE = mf104[CURRENT_PRESET].getValue(CURRENT_PARAM_INDEX);
    printEditLayout();
    delay(400);
  }

}

int getKnobValue(int port)
{
  // -1: null, don't send midi data
  // 0-127: send midi data
   int sensorValue = analogRead(port);
   return (sensorValue * (128.0/1023.0)) - 1;
}

int PREVIOUS_MILLIS = 0;
void blink(){
  int blink_interval = 500;
  int current_millis = millis();
  if (CURRENT_PRESET == 1) {
    if(current_millis - PREVIOUS_MILLIS > blink_interval) {
      digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
      PREVIOUS_MILLIS = current_millis;
    }
  } else if (CURRENT_PRESET == 2) {
    if(current_millis - PREVIOUS_MILLIS > blink_interval) {
      digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
      PREVIOUS_MILLIS = current_millis;
    }
  } else if (CURRENT_PRESET == 3) {
    if(current_millis - PREVIOUS_MILLIS > blink_interval) {
      digitalWrite(LED3_PIN, !digitalRead(LED3_PIN));
      PREVIOUS_MILLIS = current_millis;
    }
  }
}

void printPerformanceLayout(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Performance Mode");
  lcd.setCursor(0,1);
  lcd.print("preset: ");
  lcd.print(CURRENT_PRESET);
}

void printEditLayout(){
  int cc = mf104[CURRENT_PRESET].getCC(CURRENT_PARAM_INDEX);
  lcd.clear();
  lcd.setCursor(0,0);
  String name;
  mf104[CURRENT_PRESET].getName(name, CURRENT_PARAM_INDEX);
  lcd.print(name);
  lcd.setCursor(0,1);
  lcd.print(cc); 
  lcd.print(":");
  printValue();
}

void printValue(){
  int cc = mf104[CURRENT_PRESET].getCC(CURRENT_PARAM_INDEX);
  lcd.setCursor(6,1);
  if (CURRENT_PARAM_VALUE < 0) {
    lcd.print("null");
  } else {
    String output = "";
    mf104[CURRENT_PRESET].displayOutput(output, cc, CURRENT_PARAM_VALUE);
    if (output.length() == 0){
      lcd.print(CURRENT_PARAM_VALUE);
    } else {
      lcd.print(output);
    }
  }
  lcd.print("            ");
}

void printWelcome(){
  lcd.setCursor(0,0);
  lcd.print("   V O N G O N  ");
  lcd.setCursor(0,1);
  lcd.print("  V O N G O N   ");
}
