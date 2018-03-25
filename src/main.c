/**
    Element clock firmware.

    @author Zac Crites
    @date   August 5, 2016
*/

#include <avr/interrupt.h>

#include "io.h"
#include "clock.h"
#include "display.h"

#include <util/delay.h>

/**
    Firmware entry point.
*/
int main()
{
    setupChipIo();
    setupClockTimer();
    setupDisplayTimer();
    sei();

    // The display should blink at powerup to indicate power failure.
    setDisplayBlink(true);

    while (true)
    {
        clockCheckSpeedMode();
        drawDisplay();

        // Stop blinking if speed mode is ever used to set the clock.
        if (isSpeedButtonPressed())
        {
            setDisplayBlink(false);
        }
    }

}
