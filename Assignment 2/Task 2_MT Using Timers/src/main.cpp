#include <Arduino.h>

#define CPU_HZ 48000000

#define LED_BLUE    PIN_LED_13
#define LED_YELLOW  PIN_LED_RXL

void startTimer(int, TcCount16* , IRQn, void (*f)());
void setTimerFrequency(int, TcCount16*);

void setUpTC3Clock();
void setUpTC4Clock();

void TC3_Handler();
void TC4_Handler();

void setup() {

  SerialUSB.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  // start Timers 
  startTimer(1, (TcCount16*) TC3, TC3_IRQn, setUpTC3Clock); // yellow
  startTimer(2, (TcCount16*) TC4, TC4_IRQn, setUpTC4Clock); // blue

}

void loop() { }

void setUpTC3Clock() {
  GCLK->CLKCTRL.reg = (uint16_t)
    (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
    GCLK_CLKCTRL_ID_TCC2_TC3);
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

}

void setUpTC4Clock() {
  GCLK->CLKCTRL.reg = (uint16_t)
    (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
    GCLK_CLKCTRL_ID_TC4_TC5);
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

}

void startTimer(int freqHz, TcCount16* TC, IRQn Interrpt, void (*setClock)()) {

  setClock();

  TC->CTRLA.reg = 0x0ul << TC_CTRLA_ENABLE_Pos;
  while(TC->STATUS.bit.SYNCBUSY == 1);

  TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1024;
  while(TC->STATUS.bit.SYNCBUSY == 1);

  setTimerFrequency(freqHz, TC);

  // enable compare interrupt to happen
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(Interrpt);

  TC->CTRLA.reg |= 0x1ul << TC_CTRLA_ENABLE_Pos; // TC_CTRLA_ENABLE_Pos which is 1
  while(TC->STATUS.bit.SYNCBUSY == 1);
}

void setTimerFrequency(int freqHz, TcCount16* TC) {
  int compareVal = (CPU_HZ / (1024 * freqHz)) - 1;

  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareVal); 

  // Step 4: Set the TC4 period (frequency)
  TC->CC[0].reg  = compareVal;

  SerialUSB.println(TC->COUNT.reg);
  SerialUSB.println(TC->CC[0].reg);

  while(TC->STATUS.bit.SYNCBUSY == 1);
}

void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  if(TC->INTFLAG.bit.MC0 == 1){
    TC->INTFLAG.bit.MC0 = 1;

    byte h_l = digitalRead(LED_YELLOW);
    digitalWrite(LED_YELLOW, !h_l);
    SerialUSB.print("Yellow LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");
  }
}

void TC4_Handler() {
  TcCount16* TC = (TcCount16*) TC4;
  if(TC->INTFLAG.bit.MC0 == 1){
    TC->INTFLAG.bit.MC0 = 1;

    byte h_l = digitalRead(LED_BLUE);
    digitalWrite(LED_BLUE, !h_l);
    SerialUSB.print("BLUE LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");
  }
}