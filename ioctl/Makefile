obj-m+=study_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CC) ioctl_test.c -o ioctl_test
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm ioctl_test 
