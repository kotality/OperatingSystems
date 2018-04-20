obj-m += pa4a1.o 
obj-m += pa4b1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
make -C /lib/modules/$(shell uname -r)/build M=$(PwD) clean
