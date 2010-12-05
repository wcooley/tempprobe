/* Simple analog to digitial conversion, similar to Wiring/Arduino
 *
 * This file was taken from the 'lighty' package aka. Sean Bruno's
 * "CS572_Demo_Firmware", so it is presumably (C) Copyright Sean Bruno.
 *
 * However, parts are nearly identical to pins_teensy.c installed from the
 * "Teensyduino installer, so probably it should also carry the following
 * copyright:
 *
 * >Copyright (c) 2008 PJRC.COM, LLC
 * >
 * >Permission is hereby granted, free of charge, to any person obtaining a copy
 * >of this software and associated documentation files (the "Software"), to deal
 * >in the Software without restriction, including without limitation the rights
 * >to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * >copies of the Software, and to permit persons to whom the Software is
 * >furnished to do so, subject to the following conditions:
 * >
 * >The above copyright notice and this permission notice shall be included in
 * >all copies or substantial portions of the Software.
 * >
 * >THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * >IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * >FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * >AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * >LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * >OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * >THE SOFTWARE.
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "analog.h"


#if defined(__AVR_ATmega32U4__)

uint8_t analog_reference_config_val = 0x40;

static const uint8_t PROGMEM adc_mapping[] = {
        0, 1, 4, 5, 6, 7, 13, 12, 11, 10, 9, 8
};

int analogRead(uint8_t pin)
{
        uint8_t low, adc;

        if (pin >= 12) return 0;
        adc = pgm_read_byte(adc_mapping + pin);
        if (adc < 8) {
                DIDR0 |= (1 << adc);
                ADCSRB = 0;
                ADMUX = analog_reference_config_val | adc;
        } else {
                adc -= 8;
                DIDR2 |= (1 << adc);
                ADCSRB = (1<<MUX5);
                ADMUX = analog_reference_config_val | adc;
        }
	ADCSRA = (1<<ADSC)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
        while (ADCSRA & (1<<ADSC)) ;
        low = ADCL;
        return (ADCH << 8) | low;
}

#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)

uint8_t analog_reference_config_val = 0x40;

int analogRead(uint8_t pin)
{
        uint8_t low;

	if (pin >= 8) return 0;
        DIDR0 |= (1 << pin);
        ADMUX = analog_reference_config_val | pin;
	ADCSRA = (1<<ADSC)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
        while (ADCSRA & (1<<ADSC)) ;
        low = ADCL;
        return (ADCH << 8) | low;
}

#endif

