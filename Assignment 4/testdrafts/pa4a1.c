/* 
	Novaira Farnaz, Kenia Castro, Sandy Demian
	Prog Assn #4 - Character-mode Linux driver
	COP 4600, Spring 2018
*/
//Kernel module for input device

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h> //to register device to kernel
#include <linux/semaphore.h>
#include <linux/fs.h> //for open/close, read/write to device
#include <asm/uaccess.h> //copy to user, from user (userSpace Kspace)
#include <linux/mutex.h>
#include <linux/vmalloc.h>

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
#define DEVICE_NAME "drgerberdev" //name of our device driver
#define BUF_LEN 1024  // the buffer size

struct cdev *mcdev; //our character driver
static int major_number;
int retv; //return values of kernel functions.
dev_t dev_num;  //holds major number and minor number

static char *buf;
static char *buf_Ptr;
static int *buf_index;
static int Dev_open = 0;
static int *ucflocations; // to save the indexes of "UCF"s that are in the buffer already
static int locindex = 0;

EXPORT_SYMBOL(buf);
EXPORT_SYMBOL(buf_index);
EXPORT_SYMBOL(ucflocations);
static DEFINE_MUTEX(drgerberdev_mutex);

struct file_operations fopi ={
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_close,
	.write = dev_write,
	.read = dev_read
};

//Initialization: registers a character device driver to the system
static int dev_entry(void){
		buf = (char *)vmalloc(sizeof(char)*BUF_LEN);
		buf[0] = '\0';
		buf_index = (int *)vmalloc(sizeof(int)*2);
		buf_index[0] = 0;
		ucflocations = (int *)vmalloc(sizeof(int)*28);

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

	// Initialize mutex lock
	mutex_init(&drgerberdev_mutex);

	return 0;
}

//Deinitialization function
static void dev_exit(void) {

	// Destroy allocated mutex
	mutex_destroy(&drgerberdev_mutex);

	cdev_del(mcdev);
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "unloaded module");
}

// Open
static int dev_open(struct inode *inode, struct file *filp) {
	
	static int count = 0;
	
	if (Dev_open)
		return -EBUSY;

	// Try to acquire the mutex (put lock on) Return 1 if succes; 0 contention
	if (!mutex_trylock(&drgerberdev_mutex))
	{
		printk(KERN_ALERT "drgerberdev: Device in use by another process");
		return -EBUSY;
	}
	
	Dev_open++;
	printk(KERN_INFO "Device has been opened %d times\n", count++);

	buf_Ptr = buf;
	try_module_get(THIS_MODULE);

	return 0;
}

// Release
static int dev_close(struct inode *inode, struct file *filp){
	
	// Release the mutex
	mutex_unlock(&drgerberdev_mutex);

	Dev_open--; // Ready for next caller
	
	printk(KERN_INFO "Goodbye!\n");
	module_put(THIS_MODULE);
	
	return 0;
}

// read from the buffer
static ssize_t dev_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
	
	printk(KERN_INFO "Input device - Reading not supported");

	return -EFAULT;
}

// write to the buffer
static ssize_t dev_write(struct file *filp, const char *buffer, size_t length, loff_t * off)
{	
	int i, j, k, l;
	char *champs = "Undefeated 2018 National Champions UCF";
	int strlength = 38;
	int leave = 0;

	// write to the buffer
    for( i=0 ; i<length && (buf_index[0]+i)<BUF_LEN ; i++)
    {
        get_user(buf[buf_index[0]+i], buffer+i);
    }

    // assignment 4
    for(j=0 ; j<buf_index[0] ; j++)
    {
    	// if "UCF" is found in the buffer
    	if((j+2)<buf_index[0] && buf[j] == 'U' && buf[j+1] == 'C' && buf[j+2] == 'F')
    	{
    		// check if this "UCF" was already there, not sure if this is the best way
    		for(l=0 ; l<28 ; l++)
    		{
    			if(j == ucflocations[l])
    				leave = 1;
    		}

    		if(leave == 1)
    		{
    			leave = 0;
    			break;
    		}

    		// UCF was the last thing in the buffer
    		if((j+3) >= buf_index[0])
    		{
    			// just add the new string to the end of the buffer
    			for(k=0 ; k<BUF_LEN || k<strlength ; k++)
    			{
    				get_user(buf[buf_index[0]+k], champs+k);
    			}
    			ucflocations[locindex++] = buf_index[0]+k-3;
    		}
    		// there are other stuff in the buffer
    		else
    		{
    			int s = i+3;
    			// make room for our new string
    			while(s < buf_index[0] && (s+strlength-3) < BUF_LEN)
    			{
    				get_user(buf[buf_index[0]+s+strlength-3], buf+s);
    			}
    			// write our new string
    			for(k=0 ; k < BUF_LEN || k < strlength ; k++)
    			{
    				get_user(buf[buf_index[0]+i+3+k], champs+k);
    			}
    			ucflocations[locindex++] = k+i;
    		}
    		// update i which is the charachters written
    		i = i+k;
    	}

    }

    // update the buffer index
    buf_index[0] = buf_index[0]+i;

    printk(KERN_INFO "Wrote %d characters to the buffer: [%s]", i, buf);

	return length;
}

//Caling functions
module_init(dev_entry);
module_exit(dev_exit);
