
#include "io.h"
#include <util/delay.h>


#define SHIFT_CLOCK_DELAY_US  2


void setupChipIo()
{
    // Enable outputs
    DDRA |= IO_PIN_SHIFT_DATA;
    DDRA |= IO_PIN_SHIFT_CLOCK;
    DDRA |= IO_PIN_SHIFT_LATCH;
    DDRA |= IO_PIN_SHIFT_CLEAR;

    // Enable pullups on inputs
    PORTA |= IO_PIN_ELEMENT_MODE_SWITCH;
    PORTA |= IO_PIN_SPEED_BUTTON;
}


bool isSpeedButtonPressed()
{
    return ! (PINA & IO_PIN_SPEED_BUTTON);
}


bool isElementModeSelected()
{
    return ! (PINA & IO_PIN_ELEMENT_MODE_SWITCH);
}


void shiftOutBit(bool bit)
{
    if (bit)
    {
        PORTA |= IO_PIN_SHIFT_DATA;
    }
    else
    {
        PORTA &= ~IO_PIN_SHIFT_DATA;
    }

    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= IO_PIN_SHIFT_CLOCK;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~IO_PIN_SHIFT_CLOCK;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}


void latchShiftRegister()
{
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= IO_PIN_SHIFT_LATCH;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~IO_PIN_SHIFT_LATCH;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}


void clearShiftRegister()
{
    // Register clear is active low
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~IO_PIN_SHIFT_CLEAR;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= IO_PIN_SHIFT_CLEAR;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}
