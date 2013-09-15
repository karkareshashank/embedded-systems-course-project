/*  Mydriver.c ---- driver for /dev/gmem
 *  ----------------------------------------
 *  Implemets open, close, read, write operations
 *  for gmem virtual device.
 */ 

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/param.h>
#include <linux/jiffies.h>


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Shashank Karkare");



#define DEVICE_NAME                 	"gmem"	 // Device name 
#define MEMORY_BUFFER_SIZE		256	 // Size of the buffer 
rwlock_t  mr_rwlock;				 /* Lock for simultaneous read/write purpose 
						  * Concurrency issue			    
						  */


/* per device structure */
struct My_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	char *mem_buffer;		/* buffer for the input string */
	uint8_t  pos;			/* current position in the buffer */
} *my_devp;

static dev_t my_dev_number;      	/* Allotted device number */
struct class *my_dev_class;          	/* Tie with the device model */


/*
* Open My driver
*/
int My_driver_open(struct inode *inode, struct file *file)
{
	struct My_dev *my_devp;
	
//	printk("\nopening\n");

	/* Get the per-device structure that contains this cdev */
	my_devp = container_of(inode->i_cdev, struct My_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = my_devp;

	printk("\n%s is openning\n", my_devp->name);
	return 0;
}



/*
 * Release My driver
 */
int My_driver_release(struct inode *inode, struct file *file)
{
	struct My_dev *my_devp = file->private_data;
	
	printk("\n%s is closing\n", my_devp->name);
	
	return 0;
}



/*
 * Write to my driver
 * Handles concurency issue using reader/writer spinlock 
 */
ssize_t My_driver_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
	char  temp_buffer[count];
	uint8_t curr_pos;
	int i;
	int res1;
	struct My_dev *my_devp = file->private_data;

	curr_pos = my_devp->pos;

	write_lock(&mr_rwlock);
	res1 = copy_from_user((void *)&temp_buffer, (void __user *)buf, count);

	for(i = 0; i < count;i++)
	{
		if(curr_pos == (MEMORY_BUFFER_SIZE - 1))
			curr_pos = 0;
		my_devp->mem_buffer[curr_pos] = temp_buffer[i];
		curr_pos++;
	}
	if(curr_pos == MEMORY_BUFFER_SIZE-1)
		my_devp->mem_buffer[0] = '\0';
	else
		my_devp->mem_buffer[curr_pos] = '\0';

	my_devp->pos = curr_pos;
	write_unlock(&mr_rwlock);

	printk("\n%s \n", my_devp->mem_buffer);

	return res1;
}


/*
 * Read to my driver
 * Handles concurency issue using reader/writer spinlock 
 */
ssize_t My_driver_read(struct file *file, char __user *buf , size_t count, loff_t *ppos)
{
	int res;

	read_lock(&mr_rwlock);
	res = copy_to_user((void __user *) buf , (void *)my_devp->mem_buffer, MEMORY_BUFFER_SIZE);
	read_unlock(&mr_rwlock);

	return res;
}


/* File operations structure. Defined in linux/fs.h */
static struct file_operations My_fops = {
    .owner = THIS_MODULE,          	 /* Owner */
    .open = My_driver_open,              /* Open method */
    .release = My_driver_release,        /* Release method */
    .read  = My_driver_read,		 /* Read method */
    .write = My_driver_write,            /* Write method */
};



/*
 * Driver Initialization
 */
int __init My_driver_init(void)
{
	int ret;			// Variable to store the return value form copy_to/from_user function
	char* init_string;		// Variable to hold the initialization string
	u64 curr_jiffy_count;		// Variable to hold the jiffies value
	long int uptime;		// Stores the uptime in seconds


		
	/* initializing the read/write lock */
	rwlock_init(&mr_rwlock);

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&my_dev_number, 0, 1, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* Populate sysfs entries */
	my_dev_class = class_create(THIS_MODULE, DEVICE_NAME);

	
	/* Allocate memory for the per-device structure */
	my_devp = kmalloc(sizeof(struct My_dev), GFP_KERNEL);
	if (!my_devp) {
		printk("Bad Kmalloc\n"); return -ENOMEM;
	}

	/* Allocate memory for the string */
	my_devp->mem_buffer = kmalloc(sizeof(char) * MEMORY_BUFFER_SIZE, GFP_KERNEL);
	if(!(my_devp->mem_buffer)){
		kfree(my_devp);
		printk("Bad kamlloc\n"); return -ENOMEM;
	}

	/* initializing the memory buffer with the default string */
	init_string = kmalloc(sizeof(char) * MEMORY_BUFFER_SIZE,GFP_KERNEL);
	if(!(init_string)){
		kfree(my_devp->mem_buffer);
		kfree(my_devp);
		printk("Bad kamlloc\n"); return -ENOMEM;
	}

	// Access the jiffies variable for the uptime in ticks
	curr_jiffy_count = jiffies;
	uptime = do_div(curr_jiffy_count,HZ);
	sprintf(init_string,"Hello world! This is shashank karkare, and this machine has worked for %llu seconds.",curr_jiffy_count);
	my_devp->pos = strlen(init_string) - 1;
	strcpy(my_devp->mem_buffer,init_string);	



	/* Request I/O region */
	sprintf(my_devp->name, DEVICE_NAME);


	/* Connect the file operations with the cdev */
	cdev_init(&my_devp->cdev, &My_fops);
	my_devp->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&my_devp->cdev, (my_dev_number), 1);

	if (ret) {
		printk("Bad cdev\n");
		return ret;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	device_create(my_dev_class, NULL, MKDEV(MAJOR(my_dev_number), 0), NULL, DEVICE_NAME);		
	

	printk("My Driver Initialized.\n");
	return 0;
}
/* Driver Exit */
void __exit My_driver_exit(void)
{
	
	/* Release the major number */
	unregister_chrdev_region((my_dev_number), 1);

	/* Destroy device */
	device_destroy (my_dev_class, MKDEV(MAJOR(my_dev_number), 0));
	cdev_del(&my_devp->cdev);
	kfree(my_devp->mem_buffer);
	kfree(my_devp);
	
	/* Destroy driver_class */
	class_destroy(my_dev_class);

	printk("My Driver removed.\n");
}

module_init(My_driver_init);
module_exit(My_driver_exit);
