#include <Arduino.h>


int setWatchDog(float);

int p = setWatchDog(6000);

void setup() {

  SerialUSB.begin(9600);

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

  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    if (PM->RCAUSE.reg & PM_RCAUSE_WDT) {

    SerialUSB.println("A Reset Has Happend");

    delay(1000);

    PM->RCAUSE.reg |= PM_RCAUSE_WDT;
  } else {

    SerialUSB.println("Nothing from the Watch Dog Timer");
  }
}


int setWatchDog(float periodMilli) {
  return log(periodMilli*1e-3 * 2048) / log(2);
}