/**
    Element clock firmware.

    @author Zac Crites
    @date   August 5, 2016
*/

#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/**
    IO pin configuration.
*/
enum
{
    SHIFT_DATA_PIN      = (1 << PA5),
    SHIFT_CLOCK_PIN     = (1 << PA4),
    SHIFT_LATCH_PIN     = (1 << PA3),
    SHIFT_CLEAR_PIN     = (1 << PA2),
    ELEMENT_MODE_PIN    = (1 << PA1),
    FAST_CLOCK_MODE_PIN = (1 << PA0),
};


/**
    Setup chip IO pins.

    Outputs:
        SHIFT_DATA_PIN      : Shift register data.
        SHIFT_CLOCK_PIN     : Shift register clock.
        SHIFT_LATCH_PIN     : Shift register latch.
        SHIFT_CLEAR_PIN     : Shift register clear (active low).

    Inputs:
        ELEMENT_MODE_PIN    : Display mode switch (active low).
        FAST_CLOCK_MODE_PIN : Clock speedup mode for setting time (active low).

*/
void setupIoPins()
{
    DDRA = SHIFT_DATA_PIN | SHIFT_CLOCK_PIN | SHIFT_LATCH_PIN | SHIFT_CLEAR_PIN;

    // Enable pullups on inputs
    PORTA |= ELEMENT_MODE_PIN | FAST_CLOCK_MODE_PIN;
}


/**
    Determine if the display mode is the Element mode.
*/
bool isElementMode()
{
    return ! (PINA & ELEMENT_MODE_PIN);
}


/**
    Determine if the clock is in speedup mode.
*/
bool isFastClockMode()
{
    return ! (PINA & FAST_CLOCK_MODE_PIN);
}


/**
    Font segment and digit select bit indexes.

    Each display LED segment is mapped to a specific pin on the combined
    shift register, as are the digit select bits.
*/
enum
{
    SEG_E           = (1 << 0),
    SEG_M           = (1 << 1),
    SEG_L           = (1 << 2),
    SEG_K           = (1 << 3),
    SEG_J           = (1 << 4),
    SEG_D           = (1 << 5),
    SEG_C           = (1 << 6),
    SEG_B           = (1 << 7),
    SEG_A           = (1 << 8),
    SEG_N           = (1 << 9),
    SEG_H           = (1 << 10),
    SEG_G           = (1 << 11),
    SEG_P           = (1 << 12),
    SEG_F           = (1 << 13),
    DIGIT_SELECT_1  = (1 << 14),
    DIGIT_SELECT_2  = (1 << 15),

    EMPTY_GLYPH     = (0),
    UNDEFINED_GLYPH = (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G | SEG_H | SEG_J | SEG_K | SEG_L | SEG_M | SEG_N | SEG_P),
};


/**
    Font data for all printable ASCII characters.

    All printable characters are supported, though some more than others.
*/
const uint16_t displayFont[] PROGMEM = {
    UNDEFINED_GLYPH,                                                     // NUL
    UNDEFINED_GLYPH,                                                     // SOH
    UNDEFINED_GLYPH,                                                     // STX
    UNDEFINED_GLYPH,                                                     // ETX
    UNDEFINED_GLYPH,                                                     // EOT
    UNDEFINED_GLYPH,                                                     // ENQ
    UNDEFINED_GLYPH,                                                     // ACK
    UNDEFINED_GLYPH,                                                     // BEL
    UNDEFINED_GLYPH,                                                     // BS
    UNDEFINED_GLYPH,                                                     // TAB
    UNDEFINED_GLYPH,                                                     // LF
    UNDEFINED_GLYPH,                                                     // VT
    UNDEFINED_GLYPH,                                                     // FF
    UNDEFINED_GLYPH,                                                     // CR
    UNDEFINED_GLYPH,                                                     // SO
    UNDEFINED_GLYPH,                                                     // SI
    UNDEFINED_GLYPH,                                                     // DLE
    UNDEFINED_GLYPH,                                                     // DC1
    UNDEFINED_GLYPH,                                                     // DC2
    UNDEFINED_GLYPH,                                                     // DC3
    UNDEFINED_GLYPH,                                                     // DC4
    UNDEFINED_GLYPH,                                                     // NAK
    UNDEFINED_GLYPH,                                                     // SYN
    UNDEFINED_GLYPH,                                                     // ETB
    UNDEFINED_GLYPH,                                                     // CAN
    UNDEFINED_GLYPH,                                                     // EM
    UNDEFINED_GLYPH,                                                     // SUB
    UNDEFINED_GLYPH,                                                     // ESC
    UNDEFINED_GLYPH,                                                     // FS
    UNDEFINED_GLYPH,                                                     // GS
    UNDEFINED_GLYPH,                                                     // RS
    UNDEFINED_GLYPH,                                                     // US

    EMPTY_GLYPH,                                                         // <space>
    (SEG_E | SEG_F),                                                     // !
    (SEG_G | SEG_H),                                                     // "
    (SEG_B | SEG_C | SEG_D | SEG_N | SEG_J | SEG_G | SEG_H),             // #
    (SEG_A | SEG_F | SEG_N | SEG_J | SEG_C | SEG_D | SEG_G | SEG_L),     // $
    (SEG_F | SEG_M | SEG_H | SEG_C),                                     // %
    (SEG_A | SEG_H | SEG_N | SEG_E | SEG_D | SEG_K | SEG_P),             // &
    (SEG_H),                                                             // '
    (SEG_A | SEG_F | SEG_E | SEG_D),                                     // (
    (SEG_A | SEG_B | SEG_C | SEG_D),                                     // )
    (SEG_P | SEG_G | SEG_H | SEG_M | SEG_L | SEG_K),                     // *
    (SEG_G | SEG_L | SEG_N | SEG_J),                                     // +
    (SEG_M),                                                             // ,
    (SEG_N | SEG_J),                                                     // -
    (SEG_K),                                                             // .
    (SEG_M | SEG_H),                                                     // /
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_M | SEG_H),     // 0
    (SEG_B | SEG_C | SEG_H),                                             // 1
    (SEG_A | SEG_B | SEG_D | SEG_E | SEG_N | SEG_J),                     // 2
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_J),                             // 3
    (SEG_B | SEG_C | SEG_F | SEG_N | SEG_J),                             // 4
    (SEG_A | SEG_C | SEG_D | SEG_F | SEG_N | SEG_J),                     // 5
    (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_N | SEG_J),             // 6
    (SEG_A | SEG_H | SEG_L),                                             // 7
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_N | SEG_J),     // 8
    (SEG_A | SEG_B | SEG_C | SEG_F | SEG_N | SEG_J),                     // 9
    (SEG_F | SEG_E),                                                     // :
    (SEG_F | SEG_E),                                                     // ;
    (SEG_H | SEG_K),                                                     // <
    (SEG_A | SEG_D),                                                     // =
    (SEG_P | SEG_M),                                                     // >
    (SEG_F | SEG_A | SEG_H | SEG_L | SEG_D),                             // ?

    (SEG_L | SEG_K | SEG_C | SEG_B | SEG_J | SEG_A),                     // @
    (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_N | SEG_J),             // A
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G | SEG_L | SEG_J),             // B
    (SEG_A | SEG_D | SEG_E | SEG_F),                                     // C
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G | SEG_L),                     // D
    (SEG_A | SEG_D | SEG_E | SEG_F | SEG_N | SEG_J),                     // E
    (SEG_A | SEG_E | SEG_F | SEG_N | SEG_J),                             // F
    (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_J),                     // G
    (SEG_B | SEG_C | SEG_E | SEG_F | SEG_N | SEG_J),                     // H
    (SEG_A | SEG_D | SEG_G | SEG_L),                                     // I
    (SEG_B | SEG_C | SEG_D | SEG_E),                                     // J
    (SEG_H | SEG_K | SEG_E | SEG_F | SEG_N),                             // K
    (SEG_D | SEG_E | SEG_F),                                             // L
    (SEG_H | SEG_B | SEG_C | SEG_E | SEG_F | SEG_P),                     // M
    (SEG_B | SEG_C | SEG_E | SEG_F | SEG_P | SEG_K),                     // N
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),                     // O
    (SEG_A | SEG_B | SEG_E | SEG_F | SEG_N | SEG_J),                     // P
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_K),             // Q
    (SEG_A | SEG_B | SEG_K | SEG_E | SEG_F | SEG_N | SEG_J),             // R
    (SEG_A | SEG_P | SEG_J | SEG_C | SEG_D),                             // S
    (SEG_A | SEG_G | SEG_L),                                             // T
    (SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),                             // U
    (SEG_H | SEG_M | SEG_F | SEG_E),                                     // V
    (SEG_B | SEG_C | SEG_K | SEG_M | SEG_E | SEG_F),                     // W
    (SEG_P | SEG_H | SEG_M | SEG_K),                                     // X
    (SEG_P | SEG_H | SEG_L),                                             // Y
    (SEG_A | SEG_H | SEG_M | SEG_D),                                     // Z
    (SEG_A | SEG_F | SEG_E | SEG_D),                                     // [
    (SEG_P | SEG_K),                                                     // \              :)
    (SEG_A | SEG_B | SEG_C | SEG_D),                                     // ]
    (SEG_G | SEG_A | SEG_B),                                             // ^
    (SEG_D),                                                             // _

    (SEG_P),                                                             // `
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_N | SEG_J),             // a
    (SEG_F | SEG_E | SEG_D | SEG_N | SEG_K),                             // b
    (SEG_N | SEG_J | SEG_E | SEG_D),                                     // c
    (SEG_B | SEG_C | SEG_D | SEG_M | SEG_J),                             // d
    (SEG_N | SEG_E | SEG_M | SEG_D),                                     // e
    (SEG_A | SEG_F | SEG_E | SEG_N),                                     // f
    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_P | SEG_J),             // g
    (SEG_F | SEG_E | SEG_N | SEG_J | SEG_C),                             // h
    (SEG_L),                                                             // i
    (SEG_B | SEG_C | SEG_D),                                             // j
    (SEG_G | SEG_L | SEG_K | SEG_H),                                     // k
    (SEG_G | SEG_L),                                                     // l
    (SEG_E | SEG_L | SEG_C | SEG_N | SEG_J),                             // m
    (SEG_E | SEG_N | SEG_K),                                             // n
    (SEG_N | SEG_J | SEG_C | SEG_D | SEG_E),                             // o
    (SEG_A | SEG_F | SEG_E | SEG_N | SEG_H),                             // p
    (SEG_A | SEG_B | SEG_N | SEG_J | SEG_F | SEG_K),                     // q
    (SEG_E | SEG_N),                                                     // r
    (SEG_A | SEG_P | SEG_J | SEG_C | SEG_D),                             // s
    (SEG_F | SEG_E | SEG_D | SEG_N),                                     // t
    (SEG_E | SEG_D | SEG_C),                                             // u
    (SEG_E | SEG_M),                                                     // v
    (SEG_E | SEG_M | SEG_K | SEG_C),                                     // w
    (SEG_P | SEG_H | SEG_M | SEG_K),                                     // x
    (SEG_G | SEG_B | SEG_J | SEG_C | SEG_D),                             // y
    (SEG_A | SEG_H | SEG_M | SEG_D),                                     // z
    (SEG_A | SEG_P | SEG_N | SEG_M | SEG_D),                             // {
    (SEG_G | SEG_L),                                                     // |
    (SEG_A | SEG_H | SEG_J | SEG_K | SEG_D),                             // }
    (SEG_M | SEG_J),                                                     // ~
    UNDEFINED_GLYPH,                                                     // DEL
};


/**
    Atomic symbol character data for elements "0" through 59.
*/
const uint8_t atomicSymbolChars[] PROGMEM = {
    '_', '_',
    'H', ' ',
    'H', 'e',
    'L', 'i',
    'B', 'e',
    'B', ' ',
    'C', ' ',
    'N', ' ',
    'O', ' ',
    'F', ' ',
    'N', 'e',
    'N', 'a',
    'M', 'g',
    'A', 'l',
    'S', 'i',
    'P', ' ',
    'S', ' ',
    'C', 'l',
    'A', 'r',
    'K', ' ',
    'C', 'a',
    'S', 'c',
    'T', 'i',
    'V', ' ',
    'C', 'r',
    'M', 'n',
    'F', 'e',
    'C', 'o',
    'N', 'i',
    'C', 'u',
    'Z', 'n',
    'G', 'a',
    'G', 'e',
    'A', 's',
    'S', 'e',
    'B', 'r',
    'K', 'r',
    'R', 'b',
    'S', 'r',
    'Y', ' ',
    'Z', 'r',
    'N', 'b',
    'M', 'o',
    'T', 'c',
    'R', 'u',
    'R', 'h',
    'P', 'd',
    'A', 'g',
    'C', 'd',
    'I', 'n',
    'S', 'n',
    'S', 'b',
    'T', 'e',
    'I', ' ',
    'X', 'e',
    'C', 's',
    'B', 'a',
    'L', 'a',
    'C', 'e',
    'P', 'r',
};


/**
    Delay timings.
*/
enum
{
    SHIFT_CLOCK_DELAY_US   = 2,
    DISPLAY_DIGIT_DELAY_MS = 2,
};


/**
    Shift a bit into the shift register.

    @param bit  True for a logic HIGH, False for logic LOW.
*/
void shiftOutBit(bool bit)
{
    if (bit)
    {
        PORTA |= SHIFT_DATA_PIN;
    }
    else
    {
        PORTA &= ~SHIFT_DATA_PIN;
    }

    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= SHIFT_CLOCK_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~SHIFT_CLOCK_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}


/**
    Latch the shift register output.
*/
void latchShiftRegister()
{
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= SHIFT_LATCH_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~SHIFT_LATCH_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}


/**
    Clear the shift register.
*/
void clearShiftRegister()
{
    // Register clear is active low
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA &= ~SHIFT_CLEAR_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
    PORTA |= SHIFT_CLEAR_PIN;
    _delay_us(SHIFT_CLOCK_DELAY_US);
}


/**
    Timekeeping variables.
*/
uint8_t hours = 0;
uint8_t minutes = 0;
volatile uint8_t seconds = 0;


/**
    Timer1 CTC match interrupt handler.
*/
ISR (TIM1_COMPA_vect)
{
    seconds++;
}


/**
    Timer CTC count targets, in increments of 64 microseconds.

    The units below are quoted because they're the seconds and
    minutes as counted by the chip clock, rather than actual
    seconds as measured by an external clock.
*/
enum
{
    TIMER_COUNT = 15625,     // 1.000 "seconds" / second
    FAST_TIMER_COUNT = 21,   // 12.40 "minutes" / second
};


/**
    Set up the hardware timer.

        "We're all here, the lights and noise are blinding.
         We hang back-- it's all in the timing."
*/
void setupTimer()
{
    TCCR1B |= (1 << WGM12);               // Configure Timer1 for CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10);  // Divide the timer clock by 64
    TIMSK1 |= (1 << OCIE1A);              // Enable the CTC match interrupt

    OCR1A = TIMER_COUNT;
}


/**
    Update the timer CTC count based on the selected timer mode.
*/
void updateTimerCount()
{
    static bool wasPreviouslyFastMode = false;
    bool isCurrentlyFastMode = isFastClockMode();

    // Reset the timer when switching to FAST_TIMER_COUNT, because if
    // TCNT1 > FAST_TIMER_COUNT at the time then it will miss the CTC
    // match and result in a ~4 second delay.
    if (isCurrentlyFastMode && ! wasPreviouslyFastMode)
    {
        TCNT1 = 0;
    }

    if (isCurrentlyFastMode)
    {
        OCR1A = FAST_TIMER_COUNT;
    }
    else
    {
        OCR1A = TIMER_COUNT;
    }

    wasPreviouslyFastMode = isCurrentlyFastMode;
}


/**
    Update the timekeeping variables.
*/
void updateTimeVariables()
{
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


/**
    Get segment data for the two symbols associated with a clock value.

    @param n            The clock value.
    @param elementMode  Specifies the mode: True for Element mode, False for Digit mode.
    @param pSymbol1     Pointer to output for Symbol 1 (the right digit)
    @param pSymbol2     Pointer to output for Symbol 2 (the left digit)
*/
void getSymbolData(uint8_t n, bool elementMode, uint16_t* pSymbol1, uint16_t* pSymbol2)
{
    size_t symbolOffset1;
    size_t symbolOffset2;

    if (elementMode)
    {
        // Element Mode
        symbolOffset1 = pgm_read_byte(atomicSymbolChars + (2 * n) + 1);
        symbolOffset2 = pgm_read_byte(atomicSymbolChars + (2 * n));
    }
    else
    {
        // Digit Mode
        symbolOffset1 = '0' + (n % 10);
        symbolOffset2 = '0' + ((n / 10) % 10);
    }

    *pSymbol1 = pgm_read_word(displayFont + symbolOffset1);
    *pSymbol2 = pgm_read_word(displayFont + symbolOffset2);
}


/**
    Firmware entry point.
*/
int main()
{
    setupIoPins();
    setupTimer();
    sei();

    while (true)
    {
        updateTimerCount();
        updateTimeVariables();

        uint16_t symbols[4];
        getSymbolData(minutes, isElementMode(), &symbols[0], &symbols[1]);
        getSymbolData(hours, isElementMode(), &symbols[2], &symbols[3]);

        for (uint8_t i = 0; i < 4; ++i)
        {
            clearShiftRegister();

            uint16_t symbol = symbols[i];
            if (i & 0x01)
            {
                symbol |= DIGIT_SELECT_1;
            }
            if (i & 0x02)
            {
                symbol |= DIGIT_SELECT_2;
            }

            shiftOutBit(symbol & DIGIT_SELECT_2);
            shiftOutBit(symbol & DIGIT_SELECT_1);
            shiftOutBit(symbol & SEG_F);
            shiftOutBit(symbol & SEG_P);
            shiftOutBit(symbol & SEG_G);
            shiftOutBit(symbol & SEG_H);
            shiftOutBit(symbol & SEG_N);
            shiftOutBit(symbol & SEG_A);
            shiftOutBit(symbol & SEG_B);
            shiftOutBit(symbol & SEG_C);
            shiftOutBit(symbol & SEG_D);
            shiftOutBit(symbol & SEG_J);
            shiftOutBit(symbol & SEG_K);
            shiftOutBit(symbol & SEG_L);
            shiftOutBit(symbol & SEG_M);
            shiftOutBit(symbol & SEG_E);

            latchShiftRegister();

            _delay_ms(DISPLAY_DIGIT_DELAY_MS);
        }

    }

}
