#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h> //to register device to kernel
#include <linux/semaphore.h>
#include <linux/fs.h> //for open/close, read/write to device
#include <asm/uaccess.h> //copy to user, from user (userSpace Kspace)

struct examp_device{
	char data[100];
	struct semaphore sem;
}virtual_device;

//global variables
struct cdev *mcdev; //our character driver
int major_number;
int retv; //return values of kernel functions.
dev_t dev_num;  //holds major number and minor number
#define DEVICE_NAME "gerberDevice" //name of our device driver

/*
int dev_open(){
}
int dev_close(){}
ssize_t dev_read(){}
*/

struct file_operations fopi ={
	.owner = THIS_MODULE,
	//.open = dev_open,
	//.release = dev_close,
	//.write = dev_write,
	//.read = dev_read
};

//Initialization: registers a character device driver to the system
static int dev_entry(void){
	retv = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME); //starts major number with 0, max minor=1
	if(retv<0) {
	  printk(KERN_ALERT "Failed to allocate a major number");
	  return retv;
	}
	major_number = MAJOR(dev_num); //extracts the major num
	printk(KERN_INFO "the major number is %d", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for creating a device file", DEVICE_NAME, major_number); //command: dmesg
	
	mcdev = cdev_alloc();
	mcdev->ops = &fopi; //initialize by passing the pointer
	mcdev->owner = THIS_MODULE;
	//cdev creation done, now add it to kernel
	retv = cdev_add(mcdev, dev_num, 1); //count = 1
	if(retv<0) {
	  printk(KERN_ALERT "Unable to add cdev to kernel");
	  return retv;
	}
	//initialize our semaphore
	sema_init(&virtual_device.sem,1); //inital value of one
	return 0;
}
//Deinitialization function
static void dev_exit(void) {

	cdev_del(mcdev);
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "unloaded module");
}
//Caling functions
module_init(dev_entry);
module_exit(dev_exit);
