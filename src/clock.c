
#include <avr/interrupt.h>

#include "io.h"


/**
    Timekeeping variables.
*/
volatile static uint8_t hours = 0;
volatile static uint8_t minutes = 0;
volatile static uint8_t seconds = 0;

uint8_t getClockHours()
{
    return hours;
}

uint8_t getClockMinutes()
{
    return minutes;
}


/**
    Clock timer CTC targets.
    Count in increments of 64 microseconds.
*/
#define TIMER_COUNT_CLOCK        15625   // count 1.000 "seconds" per real second
#define TIMER_COUNT_CLOCK_FAST   21      // count 12.40 "minutes" per real second

void setupClockTimer()
{
    TCCR1B |= (1 << WGM12);               // Configure Timer1 for CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10);  // Divide the timer clock by 64
    TIMSK1 |= (1 << OCIE1A);              // Enable the CTC match interrupt
    OCR1A = TIMER_COUNT_CLOCK;
}

ISR (TIM1_COMPA_vect)
{
    seconds++;
    if (seconds >= 60)
    {
        minutes++;
        seconds = 0;
    }

    if (minutes >= 60)
    {
        hours++;
        minutes = 0;
    }

    if (hours >= 24)
    {
        hours = 0;
    }
}



void clockCheckSpeedMode()
{
    static bool wasPreviouslyFastMode = false;
    bool isCurrentlySpeedMode = isSpeedButtonPressed();

    // Reset the timer when switching to TIMER_COUNT_CLOCK_FAST, because if
    // TCNT1 > TIMER_COUNT_CLOCK_FAST at the time then it will miss the CTC
    // match and result in a ~4 second delay.
    if (isCurrentlySpeedMode && ! wasPreviouslyFastMode)
    {
        TCNT1 = 0;
    }

    if (isCurrentlySpeedMode)
    {
        OCR1A = TIMER_COUNT_CLOCK_FAST;
    }
    else
    {
        OCR1A = TIMER_COUNT_CLOCK;
    }

    wasPreviouslyFastMode = isCurrentlySpeedMode;
}
