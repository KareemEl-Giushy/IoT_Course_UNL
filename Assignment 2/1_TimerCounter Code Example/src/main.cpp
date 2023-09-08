#include <Arduino.h>

#define CPU_HZ 48000000 // Main Clock Hz in the data sheet

void startTimer(int);
void setTimerFrequency(int);
void TC3_Handler();

void setup() {
    SerialUSB.begin(9600);
    pinMode(PIN_LED_13, OUTPUT);
    startTimer(1);
}


void loop() { }


void TC3_Handler() { //! is it reserverd Naming Convention ???

    TcCount16* TC = (TcCount16*) TC3;
    
    if(TC->INTFLAG.bit.MC0 == 1) {
        TC->INTFLAG.bit.MC0 = 1; //! I don't understand this part

        digitalWrite(PIN_LED_13, !digitalRead(PIN_LED_13));

    }
}

void startTimer(int freqHz) {

    GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3);
    
    //* ==== Diferent Clock ====
    // REG_GCLK_CLKCTRL = (uint16_t)
    // (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
    // GCLK_CLKCTRL_ID_TC4_TC5) ;
    
    while(GCLK->STATUS.bit.SYNCBUSY == 1);

    TcCount16* TC = (TcCount16*) TC3;

    TC->CTRLA.reg = 0x0ul << 1; // Disable The Timer
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

    TC->CTRLA.reg |= 0x0ul << 2; // 16-bit Counter
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

    TC->CTRLA.reg |= 0x1ul << 5; // WAVEGEN_MFRQ
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

    TC->CTRLA.reg |= 0x7ul << 8; // Prescaler 1024 Dividor
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync


    setTimerFrequency(freqHz);


    // enable interrupts
    TC->INTENSET.reg = 0;
    TC->INTENSET.bit.MC0 = 1;

    NVIC_EnableIRQ(TC3_IRQn);

    TC->CTRLA.reg |= 0x1ul << 1; // Enable Timer/Counter 3
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

void setTimerFrequency(int freqHz) {

    //! why we computed the compare value this way ?
    int compareVal = (CPU_HZ / (1024 * freqHz)) - 1; // 1024 Prescaler value that was set in the registers

    TcCount16* TC = (TcCount16*) TC3;

    // Make sure the count is in a proportional position to where it was
    // to prevent any jitter or disconnect when changing the compare value.
    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareVal); //! Why mapping ???
    TC->CC[0].reg = compareVal;
    SerialUSB.println(TC->COUNT.reg);
    SerialUSB.println(TC->CC[0].reg);
    while (TC->STATUS.bit.SYNCBUSY == 1);

}