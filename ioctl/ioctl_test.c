#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "ioctl.h"

int main( int argc, char *argv[] )
{
	char buf[256] = {0};
	int fd;
	if((fd = open("/dev/study_dev", O_RDWR)) < 0 )
		puts("open device error");

	puts("IOCTL_GET");
	if(ioctl(fd, IOCTL_GET, buf))
		puts("IOCTL_GET error");
	printf("%s\n", buf);
	
	puts("IOCTL_SET");
	if(ioctl(fd, IOCTL_SET, "user msg"))
		puts("IOCTL_SET error");

	puts("IOCTL_GET");
	if(ioctl(fd, IOCTL_GET, buf))
		puts("IOCTL_GET error");
	printf("%s\n", buf);

	exit(EXIT_SUCCESS);
}

