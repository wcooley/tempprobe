#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "usb_tempprobe.h"
int main() {
 
 
 int lighty_device;  //file desciptor for device

    //open the device
    lighty_device = open("/dev/tempprobe0", O_RDWR);
    if (lighty_device < 0) {
        perror("open");
    
    }
		
		ioctl(lighty_device, RDALL_CMD);

    //this will never execute
    exit(0);
 
 
}

