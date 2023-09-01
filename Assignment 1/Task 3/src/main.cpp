#include <Arduino.h>


int setWatchDog(float);

int p = setWatchDog(4000);

void setup() {
  

  WDT->CTRL.bit.ENABLE = 0;
  WDT->CONFIG.bit.PER = p; // 13 => 4 seconds
  WDT->INTENCLR.bit.EW = 1;
  WDT->CTRL.bit.WEN = 0;

  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(5); //32bits * (2^(n+1)) = 2048HZ

  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K;

  while(GCLK->STATUS.bit.SYNCBUSY)
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
  GCLK_CLKCTRL_GEN_GCLK2 |
  GCLK_CLKCTRL_CLKEN;


  WDT->CTRL.bit.ENABLE = 1;

  SerialUSB.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  SerialUSB.println(p);

}

void loop() {

  SerialUSB.println(p);

  // WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}


int setWatchDog(float periodMilli) {
  return log(periodMilli*1e-3 * 2048) / log(2);
}