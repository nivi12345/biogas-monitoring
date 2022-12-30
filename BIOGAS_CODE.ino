#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#include <Wire.h>
#include <hd44780.h>
LiquidCrystal_I2C lcd(0x27,16,2);

HX711_ADC LoadCell(4, 5);
HX711 scale;
float calibration_factor = -6050;
float load;
int load1;
int ir;

int temp1;
float amou;
float temp;
int tempPin = A2;
unsigned long FlowFrequency1 = 0;
unsigned long FlowFrequency2 = 0;
unsigned long litres_long1 = 0;
unsigned long litres_long2 = 0;
String on;
String off;
String detect;
String notdetect;
String gasin;
String gasout;
String temperature1;
String weight;
float mass ;
String state1;

const int AOUTpin = A1; //the AOUT pin of the methane sensor goes into analog pin A0 of the arduino
const int ledPin = 13; //the anode of the LED connects to digital pin D13 of the arduino
const int limit = 354;
int value;
String send1,send2,send3,send4,send5,send6,send7,send8;
void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.clear();

  lcd.setCursor(4, 0);
  lcd.print("BIO-GAS");
  lcd.setCursor(3, 1);
  lcd.print("AUTOMATION");
  scale.set_scale();
  pinMode(6,INPUT_PULLUP);
pinMode(A1,INPUT);
  pinMode(10,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  attachInterrupt(0, interrupt1,FALLING);
  attachInterrupt(1, interrupt2,FALLING);
  pinMode(2,OUTPUT);
   LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(44.0); // calibration factor for load cell => strongly dependent on your individual setup
    pinMode(ledPin, OUTPUT);//sets the pin as an output of the arduino

  Serial.begin(9600); // begins connection to the LCD module
  
  delay(5000);
}
void interrupt1()
{
  FlowFrequency1++;
}
void interrupt2()
{
  FlowFrequency2++;
}
void loop()
{
loadcell();
 irdetect();
 temperature();
 flowcall();
 bill();
 display1();
 gsm();
 mq4();
 LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  Serial.println("Weight[kg]:"); // print out to LCD
  Serial.print(i/-1000); // print out the retrieved value to the second row
  delay(1000);
 Serial.println(send1);
 Serial.println(send2);
 Serial.println(send3);
 Serial.println(send4);
 Serial.println(send5);
 Serial.println(send6);
 Serial.println(send7);
 
}
void loadcell()
{
 LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  Serial.println("Weight[kg]: "); // print out to LCD
  mass =(i/-6050); // print out the retrieved value to the second row
 weight=String(mass);
 send4="WEIGHT:"+weight+"kg\n";
}
void mq4(){
  value = analogRead(AOUTpin); //reads the analaog value from the methane sensor's AOUT pin
  Serial.print("Methane value: ");
  Serial.println(value);//prints the methane value
  delay(5000);
  if (limit <value) {
    digitalWrite(ledPin, HIGH);//if limit has been reached, LED turns on as status indicator
      Serial.print("state : danger ");

   off = "leak";
    
   

  }
  else {
    digitalWrite(ledPin, LOW);//if threshold not reached, LED remains off
    Serial.print("state : safe ");
       off = "no leak";
      
        
  }
  }
void irdetect()
{
ir=digitalRead(6);
Serial.print("\nIR:");
Serial.println(ir);
if(ir==0)
{
 relay1();
}
else
{
 relay2();
}
}
void temperature()
{
temp = analogRead(tempPin);
   // read analog volt from sensor and save to variable temp
   temp = temp * 0.48828125;
   // convert the analog volt to its temperature equivalent
   Serial.print("TEMPERATURE = ");
   Serial.print(temp); // display temperature value
   Serial.print("*C");
   Serial.println();
Serial.println(temp);
temperature1=String(temp);
send3="TEMPERATURE:"+temperature1+"C";
}
void flowcall()
{
litres_long1=FlowFrequency1;
Serial.print("litres_long1:");
Serial.println(litres_long1);
gasin=String(litres_long1);
send1="GASIN:"+gasin+"m3";
litres_long2=FlowFrequency2;
Serial.print("litres_long2:");
Serial.println(litres_long2);
gasout=String(litres_long2);
send2="GASOUT:"+gasout+"m3";
}
void bill(){
    amou=litres_long2*0.01;
   Serial.print("bill: ");
 Serial.print(amou);
 Serial.print(" rs");
String amount= String(amou);
 send8="bill"+amount+"rs";
  }
void relay1()
{
  digitalWrite(2,HIGH);
  on="ON";
  detect="PRESENT";
  send5="BAG:"+detect;
  send6="MOTOR:"+on;
}
void relay2()
{
  digitalWrite(2,LOW);
  on="OFF";
  detect="NOT PRESENT";
  send5="BAG:"+detect;
  send6="MOTOR:"+on;
}
void gsm()
{
  parseCmd("AT"); //Check AT
  parseCmd("AT+CMGD=1");    // Delete the sms
  parseCmd("AT+CMGF=1");    // Text Mode
  parseCmd("AT+CMGS=\"+918608585975\"");
  delay(300);
  parseCmd(send1);
  delay(50);
  parseCmd(send2);
  delay(50);
  parseCmd(send3);
  delay(50);
  parseCmd(send4);
  delay(50);
  parseCmd(send5);
  delay(50);
  parseCmd(send6);
  delay(50);
  parseCmd(send7);
  Serial.write((char)26);
}
void parseCmd(String command)
{
  Serial.println(command);
  delay(200);
  while (Serial.available() > 0)
  {
    Serial.print(char(Serial.read()));
  }
}
void display1()
{
lcd.backlight();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("State:");
lcd.setCursor(7,0);
lcd.print(off);
delay(1500);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("GAS IN:");
lcd.setCursor(7, 0);
lcd.print(litres_long1);
lcd.setCursor(14, 0);
lcd.print("m3");
lcd.setCursor(0, 1);
lcd.print("GAS OUT:");
lcd.setCursor(8, 1);
lcd.print(litres_long2);
lcd.setCursor(14, 1);
lcd.print("m3");
delay(1500);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("TEMPERATURE:");
lcd.setCursor(12, 0);
lcd.print(temperature1);
lcd.setCursor(15, 0);
lcd.print("C");
lcd.setCursor(0, 1);
lcd.print("WEIGHT:");
lcd.setCursor(8, 1);
lcd.print(load);
lcd.setCursor(14, 1);
lcd.print("kg");
delay(1500);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("BAG:");
lcd.setCursor(4, 0);
lcd.print(detect);
lcd.setCursor(0, 1);
lcd.print("MOTOR:");
lcd.setCursor(6, 1);
lcd.print(on);
delay(1500);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("bill");
lcd.setCursor(0,1);
lcd.print(amou);
lcd.setCursor(5,0);
lcd.print("rupees");
delay(1500);
lcd.clear();
}
