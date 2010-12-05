/*
 * readtemp - Read and print the data from the temperature probes
 *
 * Copyright (c) 2010 by Freddy Carl, Wil Cooley and Akbar Saidov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "usb_tempprobe.h"

#define DEVICE_FILE 	"/dev/misc/tempprobe0"
#define BUFSIZE		1024

void usage(char *, int);

int
main(int argc, char **argv) {

	int len;
	int tempprobe_device;  // file desciptor for device
	char buf[BUFSIZE];

	if (argc != 2)
		usage(argv[0], 1);

	memset(buf, 0, BUFSIZE);

	tempprobe_device = open(DEVICE_FILE, O_RDWR);
	if (tempprobe_device < 0) {
		perror("Failed to open device file '" DEVICE_FILE "'");
		exit(1);
	}

	switch (argv[1][0]) {
	case 'a':
		ioctl(tempprobe_device, ENUM_CMD);
		break;
	case 'b':
		ioctl(tempprobe_device, RDALL_CMD);
		break;
	default:
		fprintf(stderr, "Unrecognized parameter: '%s'\n", argv[1]);
		usage(argv[0], 1);
	}

	len = read(tempprobe_device, buf, BUFSIZE);
	if (len == -1) {
		perror("Error reading from device");
		exit(1);
	}
	
	printf("%s\n", buf);

}

void
usage(char *prog, int exitval) {
	fprintf(stderr, "Usage: %s [ a | b ]\n", prog);
	fprintf(stderr, " * a - Enumerate probes\n");
	fprintf(stderr, " * b - Read data\n");
	exit(exitval);
}
