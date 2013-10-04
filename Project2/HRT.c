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
#include <linux/param.h>
#include <plat/dmtimer.h>



#define DRIVER_LICENSE			"GPL v2"
#define DRIVER_AUTHOR			"Shashank Karkare"
#define DRIVER_DESCRIPTION		"It is a High Resolution Timer using system clock"
#define DEVICE_NAME                     "HRT"	 						// Device name


/* per device structure */
struct My_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[20];                  /* Name of device*/
	struct omap_dm_timer *hr_timer;	/* contains the timer associated with the open */
} *my_devp;

static dev_t my_dev_number;      	/* Allotted device number */
struct class *my_dev_class;         /* Tie with the device model */



/*
* Open My driver
*/
int My_driver_open(struct inode *inode, struct file *file)
{
	struct My_dev *my_devp;
	
	/* Get the per-device structure that contains this cdev */
	my_devp = container_of(inode->i_cdev, struct My_dev, cdev);
	
	/* For easy access of the my_dev structure */
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
 * Read the time from the timer
 */
ssize_t My_driver_read(struct file *file, char __user *buf , size_t count, loff_t *ppos)
{
	unsigned int counter_value;
	int res;
	struct My_dev* my_devp = file->private_data;

	// Read from the timer here //
	counter_value = omap_dm_timer_read_counter(my_devp->hr_timer);
	
	res = copy_to_user((void __user *) buf , (void *)&counter_value , sizeof(unsigned int));
	

	return res;
}


/*
 *  Exporting read_hrt_counter function for other module to access it
 */
unsigned int read_hrt_counter(void)
{
	unsigned int count;
	
	count = omap_dm_timer_read_counter(my_devp->hr_timer);
	
	return count;
	
}
EXPORT_SYMBOL_GPL(read_hrt_counter);



/*
 *  I/O control operations on teh timer
 */
static long My_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct My_dev* my_ptr =  file->private_data;
	
	switch(cmd)
	{
		case 0xffc1:
				omap_dm_timer_start(my_ptr->hr_timer);
				printk("timer = %d \n",omap_dm_timer_read_counter(my_ptr->hr_timer));
				break;
		case 0xffc2:
				omap_dm_timer_stop(my_ptr->hr_timer);
				break;
		case 0xffc3:
				omap_dm_timer_write_counter(my_ptr->hr_timer,arg);
				break;
		default:
				printk("HRT: unknown argument to the ioctl function\n");
				return -EINVAL;
	};


	return 0;
}

/* File operations structure. Defined in linux/fs.h */
static struct file_operations My_fops = {
    .owner = THIS_MODULE,          	 	/* Owner */
    .open = My_driver_open,          	/* Open method */
    .release = My_driver_release,    	/* Release method */
    .read  = My_driver_read,		 	/* Read method */
    .unlocked_ioctl = My_driver_ioctl,	/* I/O control method */
};



/*
 * Driver Initialization
 */
int __init My_driver_init(void)
{
	int ret;			// Variable to store the return value form copy_to/from_user function
	
		
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

	 ///// Allocate the timer  here //////
        my_devp->hr_timer = omap_dm_timer_request();
        if(my_devp->hr_timer == NULL)
        {
                printk("HRT: No more gp timers are available \n");
		kfree(my_devp);
                return -1;
        }

        // Setting souce clock for the timer //
        omap_dm_timer_set_source(my_devp->hr_timer,OMAP_TIMER_SRC_SYS_CLK);

        // Setting timer prescaler to 0 (1:1) //
        omap_dm_timer_set_prescaler(my_devp->hr_timer,0);


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
	/* Freeing the dm_timer */
	omap_dm_timer_free(my_devp->hr_timer);
	
	/* Release the major number */
	unregister_chrdev_region((my_dev_number), 1);

	/* Destroy device */
	device_destroy (my_dev_class, MKDEV(MAJOR(my_dev_number), 0));
	cdev_del(&my_devp->cdev);
		
	/* Destroy driver_class */
	class_destroy(my_dev_class);

	/* destroying the data structure */
	kfree(my_devp);

	printk("My Driver removed.\n");
}

module_init(My_driver_init);
module_exit(My_driver_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

