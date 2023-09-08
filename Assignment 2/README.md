# Assignment 2: Multi-Tasking

**How to Apply multi-tasking on embedded systems Using:**
- Time Intervals
- Timers/Counters (TC[x])

***!! If you are using the PDF version of the documentation, I encourage you to check out the repo with the documentation well-presented !!***

[Repo on github](https://github.com/KareemEl-Giushy/IoT_Course_UNL/tree/main/Assignment%202)

## Task 1: Multi-Tasking Using Millis function

### *1.1. Task Requirements:*
* Toggling two LEDs
    * Turn on the yellow LED for 1 second, and turn it off for 1 second.

    * Turn on the blue LED for 0.5 seconds, and turn it off for 0.5 seconds.

    * Print out a message every time an LED changes its state:
        ```
        “Yellow LED is on/off.”
        “Blue LED is on/off.”
        ```

* Use only ```millis()``` and time comparison.

### *1.2. Development Plan:*

Used The Example in the task sheet a reference. To add some structure, each lED logic can be added to a function and this function can be called in the loop function.

| Register Name | Register Function | Register Value |
| -- | -- | -- |
| -- | -- | -- |

### *1.3. Development Process:*

While Implementing the algorithm, I faced some bugs/pitfalls/problems such as:

1. Using ```Serial``` instead of ```SerialUSB```.
2. How to get/print the state of the LED (ON/OFF):
    * To solve the problem we get the value of the LED using ```digitalRead()``` and and store the value to ```byte h_l``` stands for **h: high, l: low**.
    * When printing the value we use a [ternary if operator](https://www.freecodecamp.org/news/c-ternary-operator/).
        ```cpp
            (h_l == 1 ? "On" : "OFF")
        ```
        *This will be used in the rest of the tasks in this assignment.*

3. Solve silly naming / forgetting to use a variable.
4. Using the wrong LED (<font color="green">Green</font> instead of <font color="blue">Blue</font>).

***1.3.1. Code Snippets:***

Initialization:
```cpp
#define LED_GREEN   PIN_LED_TXL
#define LED_BLUE    PIN_LED_13
#define LED_YELLOW  PIN_LED_RXL

long perviousYMillis = 0;
long perviousBMillis = 0;

int yellowInterval = 1000; // one second
int blueInterval = 500; // half a second

void yellowLED();
void blueLED();
```

```blueLED()``` function:
```cpp
void yellowLED() {
  long currentMillis = millis();

  if((currentMillis - perviousYMillis) > yellowInterval) {
    perviousYMillis = currentMillis;

    byte h_l = digitalRead(LED_YELLOW);    
    
    digitalWrite(LED_YELLOW, !h_l);
    SerialUSB.print("Yellow LED is "); SerialUSB.println(h_l == 1 ? "On" : "OFF");
  }
}
```

```yellowLED()``` function:
```cpp
void blueLED() {
  long currentMillis = millis();

  if((currentMillis - perviousBMillis) > blueInterval) {
    perviousBMillis = currentMillis;
    
    byte h_l = digitalRead(LED_BLUE); 

    digitalWrite(LED_BLUE, !h_l);
    SerialUSB.print("Blue LED is "); SerialUSB.println(h_l == 1 ? "On" : "OFF");    
  }
}
```

### *1.4. Test Plan:*

Define what to test:
- LEDs (<font color="yellow">Yellow</font> And <font color="blue">Blue</font>).
- Time Interval **(1s, 0.5s)**.
- Log Messages in Serial

*No need for any special types of testing. Observation is enough.*

| Component | Test | Result | Comment |
| --- | --- | --- | --- |
| LED | yellow LED blinking every second | fail | used the wrong pin |
| LED | blue LED blinking every half a second | pass | --- |
| LED | yellow LED blinking every second at the same time blue LED blinking every half a second | pass | --- |
| Serial | printing log data about the LEDs | fail | used ```Serial``` instead of ```SerialUSB``` |
| Serial | printing log data about the LEDs | pass | --- |

### *1.5. Results:*

Screenshot of the serial output:
<img src="screenshots/Task 1_Serial Log.png">

***1.5.1 Appendix***
```cpp
#define LED_GREEN   PIN_LED_TXL
#define LED_BLUE    PIN_LED_13
#define LED_YELLOW  PIN_LED_RXL

long perviousYMillis = 0;
long perviousBMillis = 0;

int yellowInterval = 1000; // one second
int blueInterval = 500; // half a second

void yellowLED();
void blueLED();

void setup() {

  SerialUSB.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

}

void loop() {
  yellowLED();
  blueLED();
}

void yellowLED() {
  long currentMillis = millis();

  if((currentMillis - perviousYMillis) > yellowInterval) {
    perviousYMillis = currentMillis;

    byte h_l = digitalRead(LED_YELLOW);    
    
    digitalWrite(LED_YELLOW, !h_l);
    SerialUSB.print("Yellow LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");
  }
}

void blueLED() {
  long currentMillis = millis();

  if((currentMillis - perviousBMillis) > blueInterval) {
    perviousBMillis = currentMillis;
    
    byte h_l = digitalRead(LED_BLUE); 

    digitalWrite(LED_BLUE, !h_l);
    SerialUSB.print("Blue LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");    
  }
}
```
EOF

---
## Task 2: Multi-Tasking Using Timers
### *2.1. Task Requirements:*
* Toggling two LEDs
    * Turn on the yellow LED for 1 second, and turn it off for 1 second.

    * Turn on the blue LED for 0.5 seconds, and turn it off for 0.5 seconds.

    * Print out a message every time an LED changes its state:
        ```
        “Yellow LED is on/off.”
        “Blue LED is on/off.”
        ```
* Use 2 Timers **(TC3, TC4)** (interrupts)
    * Worknig on 16bit timer
    * Use Main Clock ```GCLK_CLKCTRL_GEN_GCLK0``` 
    * Merge unnecessary lines
    * Use Interrput Overflow instead of match interrupt (MC0)

### *2.2. Development Plan:*

Firstly, try to undersand what exactly that we are looking for. Which registers needed, how to configure them corrrectly, is the datasheet is enough to cover all needed information?

All these questions needed to be answered before start actual coding.

| Register Name | Register Function | Register Value |
| -- | -- | -- |
| TC->CTRLA.reg | Control A: Set necessary Configuration needed to start a timer | ``` TC_CTRLA_MODE_COUNT16 \| TC_CTRLA_WAVEGEN_MFRQ \| TC_CTRLA_PRESCALER_DIV1024``` |
| TC->INTENSET.bit.OVF | Enable Overflow interrupt to happen | 1 |
| TC->CC[0].reg | Set The Top value In Match Frequency Mode | ```(CPU_HZ / (1024 * freqHz)) - 1``` |
| GCLK->CLKCTRL.reg | Set the main clock to TC3 to synchronize | ```(GCLK_CLKCTRL_CLKEN \| GCLK_CLKCTRL_GEN_GCLK0 \| GCLK_CLKCTRL_ID_TCC2_TC3)``` |
| GCLK->CLKCTRL.reg | Set the main clock to TC4 to synchronize | ```(GCLK_CLKCTRL_CLKEN \| GCLK_CLKCTRL_GEN_GCLK0 \| GCLK_CLKCTRL_ID_TC4_TC5)``` |


### *2.3. Development Process:*

While Implementing the algorithm, I faced some bugs/pitfalls/problems such as:

1. How to make the code more dynamic more then the example code. What I end up doing is:
    - Pass Timer/Counter and any other part that can be changed into the function while clocking. Examples:
        ```cpp 
        /*
        int => frequency with Hz,
        
        TcCounter16* => pointer to the TC to be used,
        
        IRQn => any IRQn type to be passed to Nested Vector Interrupt Controller

        void (*f)() => pass a clock function to configure/syncrohnize with the clock
        */
        void startTimer(int, TcCount16* , IRQn, void (*f)());
        
        /*
        int => frequency with Hz,
        TcCounter16* => pointer to the TC
        */
        void setTimerFrequency(int, TcCount16*);
        ```
2. [How to a function as an argument we used](https://jraleman.medium.com/c-programming-language-passing-a-function-as-a-parameter-90d52fe842ea)
3. Figuring out how to configure the *same clock* for the two **TCs**.
    - Implemented each TC separately and found out they work perfectly but separately till I realized that I need to sync the Timers With the clock nothing more.
4. <font color="green">***Also, tried to work directly with register values without using ready macro directives***</font>

***2.3.1. Code Snippets:***

```startTimer()``` function:
```cpp

void startTimer(int freqHz, TcCount16* TC, IRQn Interrpt, void (*setClock)()) {

  setClock();

  TC->CTRLA.reg = 0x0ul << TC_CTRLA_ENABLE_Pos;
  while(TC->STATUS.bit.SYNCBUSY == 1);

  TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1024;
  while(TC->STATUS.bit.SYNCBUSY == 1);

  setTimerFrequency(freqHz, TC);

  // enable compare interrupt to happen
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.OVF = TC_INTENSET_OVF;

  NVIC_EnableIRQ(Interrpt);

  TC->CTRLA.reg |= 0x1ul << TC_CTRLA_ENABLE_Pos; // TC_CTRLA_ENABLE_Pos which is 1
  while(TC->STATUS.bit.SYNCBUSY == 1);
}

```

```setTimerFrequency()``` function:
```cpp
void setTimerFrequency(int freqHz, TcCount16* TC) {
  int compareVal = (CPU_HZ / (1024 * freqHz)) - 1;

  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareVal); 

  // Step 4: Set the TC4 period (frequency)
  TC->CC[0].reg  = compareVal;

  SerialUSB.println(TC->COUNT.reg);
  SerialUSB.println(TC->CC[0].reg);

  while(TC->STATUS.bit.SYNCBUSY == 1);
}
```
Clock Init Functions:
```cpp
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
```

*TC Handlers:*
```cpp
void TC3_Handler() {

  byte h_l = digitalRead(LED_YELLOW);
  digitalWrite(LED_YELLOW, !h_l);
  SerialUSB.print("Yellow LED is "); SerialUSB.println(h_l == 1 ? "On" : "OFF");

  REG_TC3_INTFLAG = TC_INTFLAG_OVF;
}

void TC4_Handler() {

  byte h_l = digitalRead(LED_BLUE);
  digitalWrite(LED_BLUE, !h_l);
  SerialUSB.print("BLUE LED is "); SerialUSB.println(h_l == 1 ? "On" : "OFF");

  REG_TC4_INTFLAG = TC_INTFLAG_OVF;
}
```
### *2.4. Test Plan:*

Define What to test:
- LEDs (<font color="yellow">Yellow</font> And <font color="blue">Blue</font>).
- Time Interval **(1s, 0.5s)**.
- Log Messages in Serial

*Testing Both Timers/Counter Separately First And Thing Combine The Two Behaviours.*


| Component | Test | Result | Comment |
| --- | --- | --- | --- |
| Timer | Test if TC3 has been initialized correctly and synchronized with the clock | fail | fail while uploading to the chip, make sure to press the reset button multipule times, don't really know what is the problem |
| Timer | Test if TC3 has been initialized correctly and synchronized with the clock | fail | The LED doesn't blink at all, maybe because of the timer interval it's to fast, or maybe it's <font color="green">because the clock isn't initalized correctly</font> |
| Timer | Test if TC3 has been initialized correctly and synchronized with the clock | pass | --- |
| Timer | Test if TC4 has been initialized correctly and synchronized with the clock | pass | --- |
| LED | Test if the blue LED blinks correctly within the time boundry  | pass | --- |
| LED | Test if the yellow LED blinks correctly within the time boundry  | pass | --- |
| System | test the the serial log messages corresponding to the LEDs | pass | --- |

### *2.5. Results:*
Screenshot of the serial output:
<img src="screenshots/Task 2_Serial Output.png">

***2.5.2 Questions***


***2.5.1 Appendix***
```cpp
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
pip install grip  

  setTimerFrequency(freqHz, TC);

  // enable compare interrupt to happen
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.OVF = TC_INTENSET_OVF;

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

  byte h_l = digitalRead(LED_YELLOW);
  digitalWrite(LED_YELLOW, !h_l);
  SerialUSB.print("Yellow LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");

  REG_TC3_INTFLAG = TC_INTFLAG_OVF;
}

void TC4_Handler() {

  byte h_l = digitalRead(LED_BLUE);
  digitalWrite(LED_BLUE, !h_l);
  SerialUSB.print("BLUE LED is ");SerialUSB.println(h_l == 1 ? "On" : "OFF");

  REG_TC4_INTFLAG = TC_INTFLAG_OVF;
}
```
EOF

---

## Task 3: Multi-Tasking Using One Timer
### *3.1. Task Requirements:*
* Toggling two LEDs
    * Turn on the yellow LED for 1 second, and turn it off for 1 second.

    * Turn on the blue LED for 0.5 seconds, and turn it off for 0.5 seconds.

    * Print out a message every time an LED changes its state:
        ```
        “Yellow LED is on/off.”
        “Blue LED is on/off.”
        ```
* Only Use one Timer Unit **(TC3)**.
* Use Normal Frequency Operation mode.
* Must be running in **COUNT8** mode.
* Use ***generic clock generator 2*** to supply a **1024Hz** clock source.

### *3.2. Development Plan:*

A compination of the two preivous tasks can be done to achive the traget state. A small interval that is a wrapper to a bigger interval.
The short interval can be made using ***TC3*** and the other long interval can be made using ```millis()```.

| Register Name | Register Function | Register Value |
| -- | -- | -- |
| TC->CTRLA.reg | Control A: Set necessary Configuration needed to start a timer | ``` TC_CTRLA_MODE_COUNT16 \| TC_CTRLA_WAVEGEN_MFRQ \| TC_CTRLA_PRESCALER_DIV1024``` |
| TC->INTENSET.bit.OVF | Enable Overflow interrupt to happen | 1 |
| TC->CC[0].reg | Set The Top value In Match Frequency Mode | ```(CPU_HZ / (1024 * freqHz)) - 1``` |
| GCLK->CLKCTRL.reg | Set the main clock to TC3 to synchronize | ```(GCLK_CLKCTRL_CLKEN \| GCLK_CLKCTRL_GEN_GCLK0 \| GCLK_CLKCTRL_ID_TCC2_TC3)``` |

### *3.3. Development Process:*

While Implementing the algorithm, I faced some bugs/pitfalls/problems such as:

1. Dividing the clock signal corretly.
    - Use this formula:
        ```
        Clock Freq = 32 bit * (2^(n+1))
        ```
2. Further divide the clock to make it slower to only one second if possible
    - <font color="red">Calculating Prescaler Using this formula: </font>
        ```
        Prescaler Value = Clock Freq * Desired Time in seconds
        ```
4. Setting up The period (IN 8 bit mode).

***2.3.1. Code Snippets:***

```startTimer()``` function:
```cpp
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
```

```setTimerFrequency()``` function:

```cpp
void setTimerFrequency(int freqHz) {
  int perVal = (CLOCK_FREQ / (PRESCALER * freqHz)) - 1;

  TcCount8* TC = (TcCount8*) TC3;

  TC->PER.reg = perVal;

  while(TC->STATUS.bit.SYNCBUSY == 1);
}
```


```setClockGenerator()``` function:

```cpp

void setClockGenerator() {  
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4); // 32 * 2^(n+1) ==> 1024
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_DIVSEL;
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_TCC2_TC3;
  while(GCLK->STATUS.bit.SYNCBUSY == 1);

}
```

```TC3_Handler()``` function:
```cpp
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
```

### *3.4. Test Plan:*
Define what to test:
- LEDs (<font color="yellow">Yellow</font> And <font color="blue">Blue</font>).
- Time Interval **(1s, 0.5s)**.
- Log Messages in Serial

*Testing each LED separately is extremely important to determine if the intended behaviour is achived or not*

| Component | Test | Result | Comment |
| --- | --- | --- | --- |
| Timer | Is The timer worknig correctly in the intended time interval | fail | need adjusting the dividers (Maybe clock, <font color="green">Maybe TC divider</font>) |
| Timer | Is The timer worknig correctly in the intended time interval | pass | --- |
| LED | Is the blue LED is blinking within the correct interval | pass | --- |
| LED | Is the yellow LED is blinking within the correct interval | pass | --- |

### *3.5. Results:*
Screenshot of the serial output:
<img src="screenshots/Task 3_Serial Out.png">

***3.5.1 Appendix***
```cpp
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
```

EOF

---