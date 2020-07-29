#include <avr/sleep.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55);

//sleep and wake buttons
#define wakePin 2
#define resetPin 3
//calibration button
#define gomb 8

//potentiomaeters for testing
#define xpot A0
#define ypot A1

//output leds
#define ledUp 9
#define ledDown 10
#define ledLeft 11
#define ledRight 5

//calibration values
int x = 0;
int y = 0;
int firstcalib = 0;

//output values
int outx = 0;
int outy = 0;
int pozityivx = 0;
int pozityivy = 0;

//tilting sensor stuff
#define buttonPin 7
#define ledPin  12
#define interrupt_LED 6
const int m_interval = 400;

int buttonState = 0;

unsigned long frequency = 0;
unsigned long diff = 0;
unsigned long value1 = 0;
unsigned long value2 = 0;

//turn off timer stuff
unsigned long previousMillis = 0;
const long interval = 500000;

void setup(void)
{
  //bno.begin();
  pinMode(gomb, INPUT_PULLUP);
  digitalWrite(wakePin, HIGH);
  //attachInterrupt(digitalPinToInterrupt(wakePin), wake, CHANGE);
  pinMode(resetPin, OUTPUT);

  //tilt stuff
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(interrupt_LED, OUTPUT);


  pinMode(ledUp, OUTPUT);
  pinMode(ledDown, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(resetPin, HIGH);
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    /*while(1){
      Blink();
      delay(600);
      }  //COMMENTED OUT INFINITE LOOP*/
  }
  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop(void)
{
  buttonState = digitalRead(buttonPin);
  unsigned long m_interval_beg = 0;
  m_interval_beg = millis();

  if (buttonState == HIGH) {
    buttonState = 1;
    //Serial.println(buttonState);
    value1 = m_interval_beg;
    diff = value1 - value2;
    digitalWrite(ledPin, HIGH);

  } else {
    buttonState = 0;
    value2 = m_interval_beg;
    diff = value2 - value1;
    //Serial.println(buttonState);
    digitalWrite(ledPin, LOW);
  }
  if (m_interval_beg >= 300) {
    if (diff < m_interval) {
      frequency++;
      delay(70);
      Serial.println("Frequency: ");
      Serial.println(frequency);
      if (frequency >= 5) {
        digitalWrite(interrupt_LED, HIGH);
        /*Serial.println("ON");
           Serial.print("Value: ");
           Serial.print(value2);
           Serial.print(", ");
           Serial.println(value1);
           Serial.print("Diff: ");
           Serial.println(diff);*/
        delay(2000);
      }
    }
    else {
      frequency = 0;
      /*Serial.println("OFF");
        Serial.print("Value: ");
        Serial.print(value2);
        Serial.print(", ");
        Serial.println(value1);
        Serial.print("Diff: ");
        Serial.println(diff);*/
      digitalWrite(interrupt_LED, LOW);
    }
  }



  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sleep();
  }
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  //normal mode
  outx = euler.x() - x;
  outy = euler.y() - y;


  //potentiometer mode
  //outx = map(analogRead(xpot), 0, 1023, 0, 360) - x;
  //outy = map(analogRead(ypot), 0, 1023, 0, 360) - y;


  if (digitalRead(gomb) == HIGH) {
    //normal mode
    firstcalib++;
    x = euler.x();
    y = euler.y();
    //outx = euler.x() - x;
    //outy = euler.y() - y;


    //pot mode
    /*x = map(analogRead(xpot), 0, 1023, 0, 360);
      y = map(analogRead(ypot), 0, 1023, 0, 360);
    */
  }


  if (outy != 0) {
    if (outy < 0) {
      pozityivx = outx * (-1);
      pozityivy = outy * (-1);

      analogWrite(ledDown, map(pozityivy, 0, 180, 0, 255)); //the values can be easily changed
      /*Serial.print("Led Down ");
        Serial.println(map(outx, -360, 0, 0, 255));*/
      analogWrite(ledUp, 0);
      delay(60);
    }
    if (outy > 0) {

      analogWrite(ledUp, map(outy, 0, 180, 0, 255));
      //Serial.print("Led Up ");
      //Serial.println(map(outx, 0, 360, 0, 255));
      analogWrite(ledDown, 0);
      delay(60);
    }
  } else {
    analogWrite(ledUp, 0);
    analogWrite(ledDown, 0);
    delay(60);
  }
  if (outx != 0) {
    if (firstcalib < 1) {

      if (outx > 180) {
        analogWrite(ledRight, map(outx, 360, 180, 0, 255));
        //Serial.print("Led Right ");
        //Serial.println(map(outy, -360, 0, 0, 255));
        analogWrite(ledLeft, 0);
        delay(60);
      }
      if (outx < 181) {
        pozityivx = outx * (-1);
        pozityivy = outy * (-1);
        analogWrite(ledLeft, map(outx, 0, 180, 0, 255));
        //Serial.print("Led Left ");
        //Serial.println(map(outy, 0, 360, 0, 255));
        analogWrite(ledRight, 0);
        delay(60);
      }
    }
    else {
      if (outx > 0) {
        pozityivx = outx * (-1);
        pozityivy = outy * (-1);
        analogWrite(ledRight, map(outx, 0, 180, 0, 255));
        //Serial.print("Led Right ");
        //Serial.println(map(outy, -360, 0, 0, 255));
        analogWrite(ledLeft, 0);
        delay(60);
      }
      if (outx < 0) {
        pozityivx = outx * (-1);
        pozityivy = outy * (-1);
        analogWrite(ledLeft, map(pozityivx, 0, 180, 0, 255));
        //Serial.print("Led Left ");
        //Serial.println(map(outy, 0, 360, 0, 255));
        analogWrite(ledRight, 0);
        delay(60);
      }
    }
  } else {
    analogWrite(ledLeft, 0);
    analogWrite(ledRight, 0);
    delay(60);
  }
  //led output pot mode
  /*if (outx != 0){
    if(outx < 0){analogWrite(ledDown, map(analogRead(xpot), 0, 1023, 0, 255));} //the values can be easily changed
    else {analogWrite(ledUp, map(analogRead(xpot), 0, 1023, 0, 255));}
    } else {
    analogWrite(ledUp, 0);
    analogWrite(ledDown, 0);
    }
    if (outy != 0){
    if(outy < 0){analogWrite(ledRight, map(analogRead(ypot), 0, 1023, 0, 255));}
    else {analogWrite(ledLeft, map(analogRead(ypot), 0, 1023, 0, 255));}
    } else {
    analogWrite(ledLeft, 0);
    analogWrite(ledRight, 0);
    }*/

  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(outx);
  Serial.print(" Y: ");
  Serial.println(outy);


  //Display the floating point data
  /*Serial.print("X: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tY: ");
    Serial.println(event.orientation.y, 4);*/

  //Serial.println(digitalRead(gomb));

  delay(100);
}
void sleep() {
  Serial.println("sleep");
  outx = 0;
  outy = 0;
  x = 0;
  y = 0;
  firstcalib = 0;
  delay(300);
  digitalWrite(resetPin, LOW);
  delay(100);
  digitalWrite(resetPin, HIGH);
  delay(900);


  bno.begin();
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(wakePin), wake, LOW);
  Serial.println("almost there");
  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
  interrupts();
  sleep_mode();

}
void wake() {
  sleep_disable();
  Serial.println("wake");
  delay(300);
  Serial.println("end");
  detachInterrupt(digitalPinToInterrupt(wakePin));
}
void Blink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(80);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(80);
  digitalWrite(LED_BUILTIN, LOW);
}
