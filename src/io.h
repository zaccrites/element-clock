/**
    AVR I/O control.

    @author Zac Crites
    @date   August 13, 2016
*/

#include <stdbool.h>

#include <avr/io.h>


/**
    IO pin configuration.
*/
#define IO_PIN_SHIFT_DATA            (1 << PA5)
#define IO_PIN_SHIFT_CLOCK           (1 << PA4)
#define IO_PIN_SHIFT_LATCH           (1 << PA3)
#define IO_PIN_SHIFT_CLEAR           (1 << PA2)
#define IO_PIN_ELEMENT_MODE_SWITCH   (1 << PA1)
#define IO_PIN_SPEED_BUTTON          (1 << PA0)


/**
    Setup chip IO pins.

    Chip Outputs:
        IO_PIN_SHIFT_DATA      : Shift register data.
        IO_PIN_SHIFT_CLOCK     : Shift register clock.
        IO_PIN_SHIFT_LATCH     : Shift register latch.
        IO_PIN_SHIFT_CLEAR     : Shift register clear (active low).

    Chip Inputs:
        IO_PIN_ELEMENT_MODE_SWITCH    : Display mode switch (active low).
        IO_PIN_SPEED_BUTTON           : Clock speedup mode for setting time (active low).

*/
void setupChipIo();


/**
    Check if the speed button is pressed.
*/
bool isSpeedButtonPressed();


/**
    Check if Element mode is selected.
*/
bool isElementModeSelected();


/**
    Shift a bit into the shift register.

    @param bit  True for a logic HIGH, False for logic LOW.
*/
void shiftOutBit(bool bit);


/**
    Clear shift register outputs.
*/
void latchShiftRegister();


/**
    Clear the shift register.
*/
void clearShiftRegister();
