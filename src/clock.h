/**
    Real-time clock control.

        "We're all here, the lights and noise are blinding.
         We hang back-- it's all in the timing."

    @author Zac Crites
    @date   August 13, 2016
*/


/**
    Setup the real time clock timer.
*/
void setupClockTimer();


/**
    Get the current real-time clock minutes.
*/
uint8_t getClockMinutes();


/**
    Get the current real-time clock hours.
*/
uint8_t getClockHours();


/**
    Set the timer CTC count based on the selected timer mode.
*/
void clockCheckSpeedMode();
