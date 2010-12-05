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

#ifndef _analog_h_included__
#define _analog_h_included__

#include <stdint.h>

#if defined(__AVR_AT90USB162__)
#define analogRead(pin) (0)
#define analogReference(ref)
#else
int16_t analogRead(uint8_t pin);
extern uint8_t analog_reference_config_val;
#define analogReference(ref) (analog_reference_config_val = (ref) << 6)
#endif

#endif
