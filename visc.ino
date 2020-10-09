
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#define RESISTOR 25 
#define TEN_BITS_IN_DECIMAL 1024
#define BUTTON 5
#define RESISTOR_VALUE 100

LiquidCrystal_I2C lcd (0x27,16,2);   

int resistorValue = 0;
float voltage = 0;
float current = 0;
float viscosity = 0;
int calibrationAddress = 0;
// int clearEEPROMCounter = 0;

// void clearEEPROM();
float calcVoltage();
float calcCurrent();
float calcViscosity(float current);

struct Calibration {
  float current;
  float viscosity;
};

Calibration calibration = { 0, 0.1 };
Calibration calibrationFromEEPROM;

float tempCSum = 0;
float tempC = 0;
int tempCounter = 0;
int LM35 = 0;

void setup () {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);

  // attachInterrupt(digitalPinToInterrupt(BUTTON), clearEEPROM, FALLING);
 
  //EEPROM.get(calibrationAddress, calibrationFromEEPROM);
  if(calibrationFromEEPROM.current > 0) calibration = calibrationFromEEPROM;

  tempCSum = analogRead(LM35);
  tempC = (1.1 * tempCSum * 100.0)/1024.0;
}

void loop() {
   /*
   * CALIBRATION
   */
   Serial.print(calibration.current);Serial.println("<- calibration.current");
   Serial.print(calibration.viscosity);Serial.println("<- calibration.viscosity");
   if(!calibration.current) {
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("Calibrar");
      lcd.setCursor(2,1);
      lcd.print("Viscosimetro");
      Serial.println("Inserte el motor en agua a 20oC y presione el botón para continuar");
      while(digitalRead(BUTTON) == HIGH);
      while(digitalRead(BUTTON) == LOW);
      
      digitalWrite(13, HIGH);
      
      current = calcCurrent();
      calibration.current = current;
      EEPROM.put(calibrationAddress, calibration);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibrado");
      Serial.println("Calibrado");
      lcd.setCursor(0,1);
      lcd.print("correctamente.");
      digitalWrite(13, LOW);
      delay(2000);
   }

   /*
   * VISCOSITY
   */
   current = calcCurrent();
   viscosity = calcViscosity(current);
   
   Serial.print("Current: "); Serial.println(current, DEC);
   Serial.print("Viscosity: "); Serial.println(viscosity, DEC);

   /*
   * TEMPERATURE
   */
   
   tempCSum += analogRead(LM35);
   tempcounter++;

   if(tempCounter == 5) {
      tempC = ((1.1 * tempCSum * 100.0)/1024.0)/5;
      tempCounter = 0;
      tempCSum = 0;
      Serial.println(tempC);
   }

   /*
   * LCD
   */
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Temp:");
   lcd.setCursor(6,0);
   lcd.print(tempC);

   if(tempC>99){
       lcd.setCursor(9,0);
       lcd.print("oC");
   } else{
       lcd.setCursor(8,0);
       lcd.print("oC");
   }

   lcd.setCursor(0,1);
   lcd.print("Visc:");
   lcd.setCursor(6,1);
   lcd.print(viscosity);

   if(viscosity>99){
       lcd.setCursor(9,0);
       lcd.print("cP");
   } else{
       lcd.setCursor(8,0);
       lcd.print("cP");
   }
   
   delay(1000);
}

float calcVoltage() {
   int value = analogRead(RESISTOR);
   float tmp = value * 5;
   return tmp / TEN_BITS_IN_DECIMAL;
}

float calcCurrent() {
   float voltage = calcVoltage();
   return voltage / RESISTOR_VALUE; 
}

float calcViscosity(float current) {
   float tmp = current * calibration.viscosity;
   return tmp / calibration.current;
}

/*void clearEEPROM() {
  clearEEPROMCounter++;
  Serial.print("EEPROM counter ");Serial.println(clearEEPROMCounter);
  if(clearEEPROMCounter == 4) {
    EEPROM.write(calibrationAddress, 0);
    calibration = { 0, 0.1 };
    Serial.println("EEPROM clean");
    clearEEPROMCounter = 0;
  }
  while(digitalRead(BUTTON) == HIGH);
}*/
