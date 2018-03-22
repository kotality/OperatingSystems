/* 
	Novaira Farnaz, Kenia Castro, Sandy Demian
	Prog Assn #2 - Character-mode Linux driver
	COP 4600, Spring 2018
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h> //to register device to kernel
#include <linux/semaphore.h>
#include <linux/fs.h> //for open/close, read/write to device
#include <asm/uaccess.h> //copy to user, from user (userSpace Kspace)

MODULE_AUTHOR("Novaira Farnaz, Kenia Castro, Sandy Demian");
MODULE_DESCRIPTION("Character-mode Linux driver as a kernal module");
MODULE_LICENSE("GPL");

struct examp_device{
	char data[100];
	struct semaphore sem;
}virtual_device;

// function prototypes
static int dev_entry(void);
static void dev_exit(void);
static int dev_open(struct inode *inode, struct file *filp);
static int dev_close(struct inode *inode, struct file *filp);
static ssize_t dev_read(struct file *filp,	char *buffer, size_t length, loff_t * offset);
static ssize_t dev_write(struct file *filp, const char *buffer, size_t length, loff_t * off);

//global variables
#define DEVICE_NAME "gerberDevice" //name of our device driver
#define BUF_LEN 1024  // the buffer size

struct cdev *mcdev; //our character driver
static int major_number;
int retv; //return values of kernel functions.
dev_t dev_num;  //holds major number and minor number

static char buf[BUF_LEN];	/* The msg the device will give when asked */
static char *buf_Ptr;
static int buf_index = 0;
static int Dev_open = 0;

struct file_operations fopi ={
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_close,
	.write = dev_write,
	.read = dev_read
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

// Open
static int dev_open(struct inode *inode, struct file *filp) {
	static int count = 0;
	
	if (Dev_open)
		return -EBUSY;
	
	Dev_open++;
	printk(KERN_INFO "Device has been opened %d times\n", count++);

	buf_Ptr = buf;
	try_module_get(THIS_MODULE);

	return 0;
}

// Release
static int dev_close(struct inode *inode, struct file *filp){
	Dev_open--; // Ready for next caller
	
	printk(KERN_INFO "Goodbye!\n");
	module_put(THIS_MODULE);
	
	return 0;
}

// read from the buffer
static ssize_t dev_read(struct file *filp,	char *buffer, size_t length, loff_t * offset){
	// l - buffer - offset
	int i = 0, j = 0, char_read;

	// return 0 if the buffer is empty
	if (buf_index == 0)
	{
		return 0;
	}

	// read from the buffer
	for (i=0 ; i<length && i<buf_index ; i++)
	{
		put_user(buf[i], buffer++);

	}
	char_read = i;

	// if we read everything in the buffer
	if (i == buf_index)
	{
		buf_index = 0;
	}
	// if we didn't read everything in the buffer
	else 
	{
		// move what is left in the buffer
		while (i < buf_index)
		{
			buf[j++] = buf[i++];
		}

		buf_index = j;
	}

	// return the number of bytes read from the buffer
	return char_read;
}

// write to the buffer
static ssize_t dev_write(struct file *filp, const char *buffer, size_t length, loff_t * off)
{	
	int i;

    for( i=0 ; i<length && (buf_index+i)<BUF_LEN ; i++)
    {
        get_user(buf[buf_index+i], buffer+i);
    }

    buf_index = buf_index+i-1;

	return 0;
}

//Caling functions
module_init(dev_entry);
module_exit(dev_exit);
