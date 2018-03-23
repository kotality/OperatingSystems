#include <stdio.h>
#include <stdlib.h>
#include<fcntl.h>

#define DEVICE "/dev/gerberdev"

int main() {
	int i, fd;
	char ch, write_buf[100], read_buf[100]; //read from dev and write to dev
	fd = open(DEVICE, O_RDWR); //open device
	if(fd == -1){
	  printf("file %s either does not exist or has been locked by another process\n", DEVICE);
	  exit(-1);
	}
	
	printf("writing a string: Hello World, my name is Sandy");
	strcpy(write_buf, "Hello World, my name is Sandy");
	write(fd, write_buf, sizeof(write_buf));

	read(fd, read_buf, 12);
	printf("First Read: %s\n", read_buf);

	read(fd, read_buf, 12);
	printf("Second Read: %s\n", read_buf);

	close(fd);
	return 0;
}