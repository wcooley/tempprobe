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
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "usb_rawhid.h"
#include "analog.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define LED_ON		(PORTD |= (1<<6))
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

void send_str(const char *s);
uint8_t recv_str(char *buf, uint8_t size);
void parse_and_execute_command(const char *buf, uint8_t num);
void send_probe_enum(void);
double thermistor_volt_to_celc(int code);

struct probe_input_record {
	uint8_t	pin;
	char *name;
};

char basic_thermistor_1[] = "basic_thermistor 1";
char basic_thermistor_2[] = "basic_thermistor 2";
char null_probe[] = "null_probe";

const struct probe_input_record probe_inputs[] = {
	/* Pin	Name 	(Input number is index) */
	{ 9, 	basic_thermistor_1 },
	{ 5, 	basic_thermistor_2 },
        { 0,    null_probe },
};

int probe_record_cnt = sizeof(probe_inputs) / sizeof(struct probe_input_record);

int main(void)
{
	uint8_t buffer[64];
	uint8_t n;
        int8_t r;
        uint8_t i;
	uint8_t len;
        int16_t pindata;
        double temp = 0.0;

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
		// if received data, do something with it
		r = usb_rawhid_recv(buffer, 0);
		if (r > 0) {
  			switch(buffer[0]){
				case 'a': //enum
					len = 0;
					LED_OFF;
					for (i=0; i < probe_record_cnt; i++) {
		                            len += sprintf_P((char *)(buffer) + len, PSTR("%d %s\n"), i, probe_inputs[i].name);
                                    	}
                                        usb_rawhid_send(buffer, 200);
					LED_ON;
					break;
				case 'b': //rdall
					len = 0;
					LED_OFF;
					for (i=0; i < probe_record_cnt; i++) {
            
                                            pindata = analogRead(probe_inputs[i].pin);
                                            
                                            temp = thermistor_volt_to_celc(pindata);
                                            
                                            // We multiply by 100 to give us 2 points of precision in integer form
                                            len += sprintf_P((char *)(buffer) + len, PSTR("%d"), lround(temp * 100));
                                            
                                            // Output tab-delimiter if this is not the first column
                                            if (probe_record_cnt > 1 && i != probe_record_cnt - 1) { strcat((char *)(buffer) + len,"\t"); len += 1;}                                        
                                        }
                                        usb_rawhid_send(buffer, 200);
					LED_ON;					
					break;
                                default:
                                        break;
                        }
                      
                          
                }
	}
}

double thermistor_volt_to_celc(int code) {
 
  double celsius;
  
  /* This code is copied directly from
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
