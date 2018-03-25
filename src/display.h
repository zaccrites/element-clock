/**
    LED display control.

    @author Zac Crites
    @date   August 13, 2016
*/

#include <stdbool.h>

/**
    Setup the display timer.
*/
void setupDisplayTimer();


/**
    Draw the display characters.
*/
void drawDisplay();


/**
    Enable or disable display blinking.

    @param shouldBlink  True if the display should blink, False otherwise.
*/
void setDisplayBlink(bool shouldBlink);
