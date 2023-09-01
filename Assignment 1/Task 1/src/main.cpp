#include <Arduino.h>

void setup() {

  WDT->CTRL.bit.ENABLE = 0;
  WDT->CONFIG.bit.PER = WDT_CONFIG_PER_2K;
  WDT->INTENCLR.bit.EW= 1; // Disable early warning interrupt
  WDT->CTRL.bit.WEN = 0;


  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(5); // => 32bits * (2^(n+1)) = 2048HZ
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K; /// | GCLK_GENCTRL_DIVSEL => What does this thing do

  while(GCLK->STATUS.bit.SYNCBUSY);
    // WDT clock = clock gen 2
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
    GCLK_CLKCTRL_CLKEN |
    GCLK_CLKCTRL_GEN_GCLK2;


  WDT->CTRL.bit.ENABLE = 1;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

}

void loop() {
}