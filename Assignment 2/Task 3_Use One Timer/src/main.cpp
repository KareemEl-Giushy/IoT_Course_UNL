#include <Arduino.h>

#define CLOCK_FREQ 1024
#define PRESCALER 1024

#define LED_BLUE    PIN_LED_13
#define LED_YELLOW  PIN_LED_RXL

void startTimer(int);
void setTimerFrequency(int);
void setClockGenerator();

void TC3_Handler();

long previousMills = 0;
int interval = 1000;

void setup() {
  SerialUSB.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  // start Timers 
  startTimer(1); // yellow
}

void loop() { }


void TC3_Handler() {
  long currentMillis = millis();

  if((currentMillis - previousMills) > interval) {
    previousMills = currentMillis;

    byte h_l_y = digitalRead(LED_YELLOW);
    digitalWrite(LED_YELLOW, !h_l_y);
    SerialUSB.print("Yellow LED is ");SerialUSB.println(h_l_y == 1 ? "On" : "OFF");
  }

    byte h_l_b = digitalRead(LED_BLUE);
    digitalWrite(LED_BLUE, !h_l_b);
    SerialUSB.print("Blue LED is ");SerialUSB.println(h_l_b == 1 ? "On" : "OFF");

  REG_TC3_INTFLAG = TC_INTFLAG_OVF;
}


void setClockGenerator() {
  
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4); // 32 * 2^(n+1) ==> 1024
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_DIVSEL;
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_TCC2_TC3;
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

}

void startTimer(int freqHz) {
  setClockGenerator();

  TcCount8* TC = (TcCount8*) TC3;

  TC->CTRLA.bit.ENABLE = 0;

  TC->CTRLA.reg = TC_CTRLA_MODE_COUNT8 | TC_CTRLA_WAVEGEN_NFRQ | TC_CTRLA_PRESCALER_DIV1024; // Further Prescaler Values
  while(TC->STATUS.bit.SYNCBUSY == 1);

  setTimerFrequency(freqHz);


  TC->INTENSET.reg = 0;
  TC->INTENSET.reg = TC_INTENSET_OVF;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.bit.ENABLE = 1;
  while(TC->STATUS.bit.SYNCBUSY == 1);
}

void setTimerFrequency(int freqHz) {
  int perVal = (CLOCK_FREQ / (PRESCALER * freqHz)) - 1;

  TcCount8* TC = (TcCount8*) TC3;

  TC->PER.reg = perVal;

  while(TC->STATUS.bit.SYNCBUSY == 1);
}