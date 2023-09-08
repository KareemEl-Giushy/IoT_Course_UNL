// Millis Approach
#include <Arduino.h>

#define LED_GREEN   PIN_LED_TXL
#define LED_BLUE    PIN_LED_13
#define LED_YELLOW  PIN_LED_RXL


long perviousYMillis = 0;
long perviousGMillis = 0;

int yellowInterval = 1000; // one second
int greenInterval = 500; // half a second

void task1();
void task2();

void setup() {

  SerialUSB.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

}

void loop() {
  task1();
  task2();
}

void task1() {
  long currentMillis = millis();

  if((currentMillis - perviousYMillis) > yellowInterval) {
    perviousYMillis = currentMillis;

    byte h_l = digitalRead(LED_YELLOW);    
    
    digitalWrite(LED_YELLOW, !h_l);
    SerialUSB.print("Yellow LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");
  }
}

void task2() {
  long currentMillis = millis();

  if((currentMillis - perviousGMillis) > greenInterval) {
    perviousGMillis = currentMillis;
    
    byte h_l = digitalRead(LED_YELLOW); 

    digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
    SerialUSB.print("Blue LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");    
  }
}