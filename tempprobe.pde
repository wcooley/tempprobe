/* Simple example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "usb_serial.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

void send_str(const char *s);
uint8_t recv_str(char *buf, uint8_t size);
void parse_and_execute_command(const char *buf, uint8_t num);
void send_probe_enum(void);

struct probe_input_record {
	uint8_t	pin;
	char *name;
};

char basic_thermistor[] = "basic_thermistor";
char null_probe[] = "null_probe";

const struct probe_input_record probe_inputs[] = {
	/* Pin	Name 	(Input number is index) */
	{ 0, 	basic_thermistor },
//        { 1,    null_probe },
};

int probe_record_cnt = sizeof(probe_inputs) / sizeof(struct probe_input_record);

#if 0
// Very simple character echo test
int main(void)
{
	CPU_PRESCALE(0);
	usb_init();
	while (1) {
		int n = usb_serial_getchar();
		if (n >= 0) usb_serial_putchar(n);
	}
}

#else

// Basic command interpreter for controlling port pins
int main(void)
{
	char buf[32];
	uint8_t n;

	// set for 16 MHz clock, and turn on the LED
	CPU_PRESCALE(0);
	LED_CONFIG;
	LED_ON;

	// initialize the USB, and then wait for the host
	// to set configuration.  If the Teensy is powered
	// without a PC connected to the USB port, this 
	// will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);

	while (1) {
		// wait for the user to run their terminal emulator program
		// which sets DTR to indicate it is ready to receive.
		while (!(usb_serial_get_control() & USB_SERIAL_DTR)) /* wait */ ;

		// discard anything that was received prior.  Sometimes the
		// operating system or other software will send a modem
		// "AT command", which can still be buffered.
		usb_serial_flush_input();

		// print a nice welcome message
		send_str(PSTR("\r\nTeensy USB Temperature Probe, "
			"Commands\r\n"
			"  enum  List available probes\r\n"
			"  rdall Read all probes and return tab-separated string\r\n"
			"  rd0-7 Read one of 0-7 possible probes\r\n\r\n"));

		// and then listen for commands and process them
		while (1) {
			send_str(PSTR("> "));
			n = recv_str(buf, sizeof(buf));
			if (n == 255) break;
                        nl();
			parse_and_execute_command(buf, n);
		}
	}
}
#endif

// Send a string to the USB serial port.  The string must be in
// flash memory, using PSTR
//
void send_str(const char *s)
{
	char c;
	while (1) {
		c = pgm_read_byte(s++);
		if (!c) { usb_serial_putchar(c); break; }
		usb_serial_putchar(c);
	}
}

void send_memstr(char *s)
{
    /*    int slen = strlen(s);
        for (int i = 0; i <= slen; i++) {
                usb_serial_putchar(s[i]);
        }
        */
        usb_serial_write((uint8_t *)&s[0], strlen(s)+1);
}        

// Receive a string from the USB serial port.  The string is stored
// in the buffer and this function will not exceed the buffer size.
// A carriage return or newline completes the string, and is not
// stored into the buffer.
// The return value is the number of characters received, or 255 if
// the virtual serial connection was closed while waiting.
//
uint8_t recv_str(char *buf, uint8_t size)
{
	int16_t r;
	uint8_t count=0;

	while (count < size) {
		r = usb_serial_getchar();
		if (r != -1) {
			if (r == '\r' || r == '\n') return count;
			if (r >= ' ' && r <= '~') {
				*buf++ = r;
				usb_serial_putchar(r);
				count++;
			}
		} else {
			if (!usb_configured() ||
			  !(usb_serial_get_control() & USB_SERIAL_DTR)) {
				// user no longer connected
				return 255;
			}
			// just a normal timeout, keep waiting
		}
	}
	return count;
}

// parse a user command and execute it, or print an error message
//
void parse_and_execute_command(const char *buf, uint8_t num)
{
	uint8_t port, pin, val;

	// Temp probe enumeration request
	if ( num == 4 && strncmp_P(buf, PSTR("enum"), num) == 0 ) {
		send_probe_enum();
		return;
	}

        if ( num == 5 && strncmp_P(buf, PSTR("rdall"), num) == 0 ) { 
                send_all_probes();
                return;
        }
        
	// otherwise, error message
	send_str(PSTR("Unknown command \""));
	usb_serial_putchar(buf[0]);
	send_str(PSTR("\", must be ? or =")); nl();
}

void nl() {
	send_str(PSTR("\r\n"));
}

void send_probe_enum() {
	int i;
	char buf[32];

	for (i=0; i < probe_record_cnt; i++) {
		sprintf_P(buf, PSTR("%d %s"), i, probe_inputs[i].name);
		send_memstr(buf);
		nl();
	}
        /* End of records is signaled by a line starting with two dashes
           and a space, followed by two "new lines" */
        send_str(PSTR("-- ")); nl(); nl();
}

void send_all_probes() {
        int i,;
        float temp, pindata;
        char buf[32];
 
 /*       
        for (i=0; i < probe_record_cnt; i++) {
            
            pindata = analogRead(probe_inputs[i].pin);
            
            temp = thermistor_volt_to_celc(pindata);
            
            // We multiply by 100 to give us 2 points of precision in integer form
            sprintf_P(buf, PSTR("%d"), round(temp * 100));
            
            // Output tab-delimiter if this is not the first column
            if ( i > 0 )  send_str(PSTR("\t"));
            
            send_memstr(buf);
        }
   */

        pindata = analogRead(0);
        temp = thermistor_volt_to_celc(pindata);
        sprintf_P(buf, PSTR("%f voltage, %f celcius"), pindata, temp);
        send_memstr(buf);
        nl();
            
}

float thermistor_volt_to_celc(int code) {
 
  float celsius;
  
  /* This code if copied directly from
     http://www.pjrc.com/teensy/tutorial4.html */
  if (code <= 289) {
    celsius = 5 + (code - 289) / 9.82;
  }
  if (code > 289 && code <= 342) {
    celsius = 10 + (code - 342) / 10.60;
  }
  if (code > 342 && code <= 398) {
    celsius = 15 + (code - 398) / 11.12;
  }
  if (code > 398 && code <= 455) {
    celsius = 20 + (code - 455) / 11.36;
  }
  if (code > 455 && code <= 512) {
    celsius = 25 + (code - 512) / 11.32;
  }
  if (code > 512 && code <= 566) {
    celsius = 30 + (code - 566) / 11.00;
  }
  if (code > 566 && code <= 619) {
    celsius = 35 + (code - 619) / 10.44;
  }
  if (code > 619 && code <= 667) {
    celsius = 40 + (code - 667) / 9.73;
  }
  if (code > 667) {
    celsius = 45 + (code - 712) / 8.90;
  }
  
  return celsius;
}
