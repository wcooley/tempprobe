#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "usb_tempprobe.h"
int main(int argc, char **argv) {
 
 int len;
 int tempprobe_device;  //file desciptor for device
 char test[1024];
 int i;
 memset(test, 0, 1024);

 if(argc != 2) exit(0);
 
     //open the device
    tempprobe_device = open("/dev/misc/tempprobe0", O_RDWR);
    if (tempprobe_device < 0) {
        perror("open");
    
    }
		
		switch(argv[1][0]) {
			case 'a':
				ioctl(tempprobe_device, ENUM_CMD);
				break;
			case 'b':
				ioctl(tempprobe_device, RDALL_CMD);
				break;
			default:
				exit(0);
		}
		
				
		len = read(tempprobe_device, test, 1024);
		
		if( len == -1 ) {
			printf("read error...\n");
			exit(1);
		}
		
		printf("%s\n", test);
				
		
    //this will never execute
    exit(0);
 
 
}

